
#ifndef CACHE_H
#define CACHE_H

#include <assert.h>
#include "util.h"
#include "memory.h"
using namespace std;

struct CacheStat {
    u32 conflict_misses = 0;
    u32 compulsory_misses = 0;
    u32 capacity_misses = 0;
    u32 cache_hits = 0;
    CacheStat() {}
    ~CacheStat() {}
};


struct CacheBlock {
    char * data = nullptr;
    u32 data_size = 0;  // length of data array
    u32 tag = 0;        // for [tag] and [dirty bit], use 1 and -2 for access mask
    u8 age = 0;         // age or fifo, using char enough for 32 and 64 byte size
    bool dirty = 0;     // dirty

    // constructor
    CacheBlock() {}

    // destructor
    ~CacheBlock() { if (data) delete[] data; }

    void set_byte(u32& reg, u32 addr) {
        assert(addr < data_size);
        data[addr] = reg;
    }

    void get_byte(u32& reg, u32 addr) {
        assert(addr < data_size);
        reg = data[addr];
    }

    char& operator [] (u32 addr) {
        assert(addr < data_size);
        return data[addr];
    }
};

ostream& operator << (ostream& os, const CacheBlock& blk) {
    os << std::hex
       << "{ tag: 0x" << blk.tag
       << ", dirty: " << blk.dirty << ", data: ";
    for (u32 d = 0; d < blk.data_size; d++) {
        os << "[0x" << (u32)(blk.data[d]) <<  "]";
    }
    return os << " }";
}

class Cache {
protected:
    u32 num_sets;       // number of associative sets
    u32 n_way;          // number of blocks per set
    CacheBlock ** sets; // 2D array cache
    Memory * ram;       // Main Memory
    u32 * fifo_ind;     // for fifo indexes

    /* helper masks and shifts */
    u32 byte_mask;   // for index of individual byte per cache block in set
    u32 assoc_mask;  // for index of set in sets
    u32 tag_mask;    // for tag comparison
    u32 assoc_shift; // amount to shift for assoc bits

    /* policies */
    bool write_back = false; // write-back or write-through
    bool LRU        = false;        // LRU or FIFO

public:
    /**
     * Constructor
     *  @param cache_size   size in bytes of the cache
     *  @param block_size    size of each cache block: number of bytes per block
     *  @param n_way        set associativity: blocks per set
     */
    explicit
    Cache(u32 cache_size, u32 block_size, u32 n_way, u32 ram_size) {
        //TO-DO: set policies

        if ( !util::is_pow2(cache_size)
             || !util::is_pow2(cache_size)
             || !util::is_pow2(n_way) )    {
            cerr << "ERROR! cache_size and block_size must be a power of 2!"
                 << endl;
            throw std::exception();
        }

        if (cache_size < (block_size * n_way)) {
            cerr << "Invalid ratio of parameters: "
                 << "cache_size : " << cache_size << endl
                 << "block_size : " << block_size << endl
                 << "n_way      : " << n_way << endl;
            throw std::exception();
        }

        num_sets = cache_size / block_size;
        this->n_way = n_way;
        sets = new CacheBlock*[num_sets];

        // initialize array
        for (u32 i = 0; i < num_sets; i++) {
            sets[i] = new CacheBlock[n_way];
            for (u32 j = 0; j < n_way; j++) {
                sets[i][j].data_size = block_size / n_way;
                sets[i][j].data = new char[block_size / n_way];
            }
        }

        byte_mask   = block_size / n_way - 1;
        assoc_shift = util::log2(block_size / n_way);
        assoc_mask  = (num_sets - 1) << assoc_shift;
        tag_mask    = ~(byte_mask | assoc_mask);

        ram = new Memory(ram_size);
        fifo_ind = new u32[num_sets](); // initialize to zeros
    }

    void print_cache() {
        // TO-DO
        cout << "num_sets = " << num_sets << endl;
        cout << "n_way    = " << n_way    << endl;
        for (u32 s = 0; s < num_sets; s++) {
            cout << "Set[" << s << "] {" << endl;
            for (u32 n = 0; n < n_way; n++) {
                cout << "    " << sets[s][n] << endl;
            }
            cout << "}" << endl;
        }
    }

    void print_statistics() {
        // TO-DO
    }

    inline void toggle_replacement() {
        LRU = !LRU;
    }

    inline void toggle_write() {
        write_back = !write_back;
    }

    // destructor
    ~Cache() {
        if (sets) {
            for (u32 s = 0; s < num_sets; s++) {
                delete[] sets[s];
            }
            delete[] sets;
        }
        if (ram) delete ram;
        if (fifo_ind) delete[] fifo_ind;
    }

    /**
     *
     */
    void store_byte(u32 &reg, u32 addr) {
        CacheBlock * set;
        u32 index;
        find_in_cache(addr, set, index);
        set[index].set_byte(reg, addr & byte_mask);
        set[index].age = 0;
    }

    void load_byte(u32 &reg, u32 addr) {
        CacheBlock * set;
        u32 index;
        find_in_cache(addr, set, index);
        set[index].get_byte(reg, addr & byte_mask);
        set[index].age = 0;
    }

protected:
    /** For LRU
     *  ages every cacheblock's age 
     */
    void update_lru(CacheBlock * set) {
        for (u32 n = 0; n < n_way; ++n) {
            //set[n].age = MAX(n_way, set[n].age + 1);
            set[n].age += 1;
        }
    }

    /**
     * For FIFO
     */
    inline void update_FIFO(u32 set_num) {
        assert(set_num < num_sets);
        fifo_ind[set_num] = (fifo_ind[set_num] + 1) % n_way;
    }

    /** find(@addr, @set, @index)
     *  returns: bool if addr is in this cache
     *
     *  behavior:
     *  if the @addr is contained in the cache, 
     *  this function sets @set to the set block
     *  and sets index to the index of the cache block
     *  in that set
     *
     *  if @addr is not in the cache, it sets index
     *  to the index of the cacheblock which is oldest
     *  for LRU replacement
     *
     *  to access the cacheblock -> set[index]
     */
    bool find_in_cache(u32 addr, CacheBlock * &set, u32 &index) {
        u32 set_ind = (addr & assoc_mask) >> assoc_shift;
        set = sets[set_ind];

        update_lru(set);
        update_FIFO(set_ind);

        index = 0;
        for (u32 p = 0; p < n_way; p++) {
            if ( (addr & tag_mask) == (set[p].tag & tag_mask) ) {
                index = p;
                set[index].age = 0;
                return true;
            }

            if (set[p].age > set[index].age) {
                index = p;
            }
        }

        index = (LRU) ? index : fifo_ind[set_ind];
        assert(index < n_way);

        // replace this block
        if (write_back && (set[index].dirty)) {
            write_block_to_memory(set + index, addr);
        }

        fetch_block_from_memory(set + index, addr);
        return false;
    }


    /**
     * copies a block of memory from ram into this cache block
     * will update the tag bits, dirty bit and age for lru
     */
    void fetch_block_from_memory(CacheBlock *block, u32 addr) {
        assert(block != nullptr);

        block->tag = block->age = block->dirty = 0; // clear block info
        block->tag = (addr & tag_mask);             // put new tag

        addr &= (~byte_mask); // change addr to where the block starts

        for (u32 p = 0; p <= byte_mask; ++p) {
            (*block)[p] = (*ram)[addr + p]; // copy ram into cache
        }
    }

    /**
     * copies a data in cache block into block of memory
     */
    void write_block_to_memory(CacheBlock *block, u32 addr) {
        assert(block != nullptr);

        block->dirty = 0;       // set dirty bit to zero
        addr &= (~byte_mask);   // change addr to where the block starts

        for (u32 p = 0; p <= byte_mask; ++p ) {
            (*ram)[addr+p] = (*block)[p]; // copy cache into ram
        }
    }
};

#endif

