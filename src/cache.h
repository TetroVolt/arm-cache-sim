
#ifndef CACHE_H
#define CACHE_H

#include <assert.h>
#include "util.h"
#include "memory.h"
using namespace std;

struct CacheBlock {
    u32 data_size = 0;
    u32 meta = 0;
        // this meta data contains meta data
        // [tag][age][dirty bit]
        // TO-DO implement proper masks and shifts
    char * data = nullptr;

    CacheBlock() {}
    ~CacheBlock() { if (data) delete[] data; }

    char& operator [](u32 index) {
        assert(index < data_size);
        return data[index];
    }
};

class Cache {
protected:
    u32 num_sets; // number of associative sets
    u32 n_way;      // number of blocks per set
    CacheBlock ** sets; // 2D array memory
    Memory * ram;

    // helper masks and shifts
    u32 byte_mask;  // for index of individual byte per cache block in set
    u32 assoc_mask; // for index of set in sets
    u32 tag_mask;   // for tag comparison
    u32 assoc_shift;

    bool write_back;

public:
    /**
     * Constructor
     *  @param cache_size   size in bytes of the cache
     *  @param block_size    size of each cache block: number of bytes per block
     *  @param n_way        set associativity: blocks per set
     */
    explicit
    Cache(u32 cache_size, u32 block_size, u32 n_way, u32 ram_size) {
        if ( !util::is_pow2(cache_size)
             || !util::is_pow2(cache_size)
             || !util::is_pow2(n_way) ) {
            cerr << "ERROR! cache_size and block_size must be a power of 2!"
                 << endl;
            throw std::exception();
        }

        if (cache_size < (block_size * n_way)) {
            cerr << "Invalid ratio of parameters: "
                 << "cache_size : " << cache_size << endl
                 << "block_size  : " << block_size << endl
                 << "n_way      : " << n_way << endl;
            throw std::exception();
        }

        num_sets = cache_size / block_size / n_way;
        sets = new CacheBlock*[num_sets];

        // initialize array
        for (u32 i = 0; i < num_sets; i++) {
            sets[i] = new CacheBlock[n_way];
            for (u32 j = 0; j < n_way; j++) {
                sets[i][j].data_size = block_size;
                sets[i][j].data = new char[block_size];
            }
        }

        byte_mask   = block_size - 1;
        assoc_mask  = (num_sets - 1) << util::log2(block_size);
        assoc_shift = util::log2(block_size);
        tag_mask    = ~(byte_mask | assoc_mask);

        ram = new Memory(ram_size);
    }

    ~Cache() {
        if (sets) {
            for (; --num_sets >= 0; delete[] sets[num_sets]);
            delete[] sets;
        }
    }

    /** For LRU
     * ages every cacheblock's age 
     */
    void age_set(CacheBlock * set) {
        //TO-DO
        //NEEDS TO BE IMPLEMENTED AND CALLED FOR STORE AND LOAD
        //META IS FLAWED, NEED MORE BITS THAN 32-TAG FOR AGES
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
        set = sets[(addr & assoc_mask) >> assoc_shift];
        u32 oldest = 0;
        for (u32 p = 0; p < n_way; p++) {
            if ( (addr & tag_mask) == (set[p].meta & tag_mask) ) {
                index = p;
                return true;
            }

            // simultaneously find oldest index for lru
            if ( ((set[p].meta & (~tag_mask)) >> 1) >
                 ((set[oldest].meta & (~tag_mask)) >> 1)) {
                oldest = p;
            }
        }
        // replace this block

        index = oldest;
        if (write_back && (set[oldest].meta & 1)) {
            write_block_to_memory(set, addr);
        }
        fetch_block_from_memory(set, addr);
        return false;
    }

    void store_byte(u32 &reg, u32 addr) {
        //TO-DO
        CacheBlock * set;
        u32 index;
        find_in_cache(addr, set, index);
        set[index][addr & byte_mask] = reg;
        set[index].meta &= (tag_mask | 1); // set age = 0
    }

    void load_byte(u32 &reg, u32 addr) {
        //TO-DO
        CacheBlock * set;
        u32 index;
        find_in_cache(addr, set, index);
        reg = set[index][addr & byte_mask]; // move byte
        reg |= (set[index][0] & 0x80) ? 0xFFFFFF00:0; // perform sign extend
        set[index].meta &= (tag_mask | 1);  // set age = 0
    }

    void load_unsigned_byte(u32 &reg, u32 addr) {
        //TO-DO
        CacheBlock * set;
        u32 index;
        find_in_cache(addr, set, index);
        reg = set[index][addr & byte_mask]; // move byte
        set[index].meta &= (tag_mask | 1);  // set age = 0
    }

    void store_word(u32 &reg, u32 addr) {
        assert(!(addr & byte_mask)); // check if word aligned
        CacheBlock *set;
        u32 index;
        find_in_cache(addr, set, index);
        *((u32*)(set[index].data)) = reg;
        if (write_back) {
            set[index].meta |= 1; // set dirty bit = 1
        } else {
            (*ram)[addr] = reg;   // write-through
        }
    }

    void load_word(u32 &reg, u32 addr) {
        assert(!(addr & byte_mask)); // check if word aligned
        CacheBlock *set;
        u32 index;
        find_in_cache(addr, set, index);
        reg = *((u32*)(set[index].data));
        set[index].meta &= (tag_mask | 1); // set age = 0
    }

protected:
    /**
     * copies a block of memory from ram into this cache block
     * will update the tag bits, dirty bit and age for lru
     */
    void fetch_block_from_memory(CacheBlock *block, u32 addr) {
        assert(block != nullptr);
        block->meta = 0;                  // clear block
        block->meta |= (addr & tag_mask); // put new tag

        addr &= (~byte_mask);  // change addr to where the block starts

        for (u32 p = 0; p <= byte_mask; ++p) {
            (*block)[p] = (*ram)[addr + p];  // copy values
        }
    }

    /**
     * copies a data in cache block into block of memory
     */
    void write_block_to_memory(CacheBlock *block, u32 addr) {
        assert(block != nullptr);
        block->meta &= -2;      // set dirty bit to 0
        addr &= (~byte_mask);   // change addr to where the block starts

        for (u32 p = 0; p <= byte_mask; ++p ) {
            (*ram)[addr+p] = (*block)[p];
        }
    }
};

#endif

