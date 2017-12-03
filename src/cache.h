
#ifndef CACHE_H
#define CACHE_H

#include <assert.h>
#include "util.h"
//#include "memory.h"
using namespace std;

/**
 * For recording statistics
 */
struct CacheStat {
    u32 conflict_misses = 0;
    u32 compulsory_misses = 0;
    u32 capacity_misses = 0;
    u32 cache_hits = 0;
    CacheStat() {}
    ~CacheStat() {}
};
ostream& operator << (ostream& os, const CacheStat& stat) {
    //TO-DO

    os << std::dec << "Cache Statistics :" << endl
       << "  conflict   misses : " << stat.conflict_misses << endl
       << "  compulsory misses : " << stat.compulsory_misses << endl
       << "  capacity   misses : " << stat.capacity_misses << endl
       << "  cache hits        : " << stat.cache_hits << endl;

    return os;
}

struct CacheBlock {
    u32 tag = 0;        // for [tag] and [dirty bit], use 1 and -2 for access mask
    u8 age = 0;         // age or fifo, using char enough for 32 and 64 byte size
    bool dirty = 0;     // dirty
    bool written_to = 0;

    // constructor
    CacheBlock() {}
    // destructor
    ~CacheBlock() {}
};

ostream& operator << (ostream& os, const CacheBlock& blk) {
    os << std::hex
       << "{ tag: 0x" << blk.tag
       << ", dirty: " << blk.dirty;
    os << ", written_to: " << blk.written_to << "}";
    return os;
}

class Cache {
protected:
    u32 num_sets;       // number of associative sets
    u32 n_way;          // number of blocks per set
    CacheBlock ** sets; // 2D array cache
    u32 * fifo_ind;     // for fifo indexes

    bool isFull = false;

    /* helper masks and shifts */
    u32 byte_mask;   // for index of individual byte per cache block in set
    u32 assoc_mask;  // for index of set in sets
    u32 tag_mask;    // for tag comparison
    u32 assoc_shift; // amount to shift for assoc bits

    /* policies */
    bool write_back = false; // write-back or write-through
    bool LRU        = false; // LRU or FIFO

    // stats
    CacheStat cache_stats;

public:
    /**
     * Constructor
     *  @param cache_size   size in bytes of the cache
     *  @param line_size    size of each cache block: number of bytes per block
     *  @param n_way        set associativity: blocks per set
     */
    explicit
    Cache(u32 cache_size, u32 line_size, u32 n_way) {

        if ( !util::is_pow2(cache_size)
             || !util::is_pow2(cache_size)
             || !util::is_pow2(n_way) )    {
            cerr << "ERROR! cache_size and line_size must be a power of 2!"
                 << endl;
            throw std::exception();
        }

        if (cache_size < (line_size * n_way)) {
            cerr << "Invalid ratio of parameters: "
                 << "cache_size : " << cache_size << endl
                 << "line_size : " << line_size << endl
                 << "n_way      : " << n_way << endl;
            throw std::exception();
        }

        num_sets = cache_size / line_size;
        this->n_way = n_way;
        sets = new CacheBlock*[num_sets];

        // initialize array
        for (u32 i = 0; i < num_sets; i++) {
            sets[i] = new CacheBlock[n_way];
        }

        byte_mask   = line_size / n_way - 1;
        assoc_shift = util::log2(line_size / n_way);
        assoc_mask  = (num_sets - 1) << assoc_shift;
        tag_mask    = ~(byte_mask | assoc_mask);

        fifo_ind = new u32[num_sets](); // initialize to zeros

        /*
        cout << "Creating new Cache with:" << endl << std::dec;
        cout << "cache_size : " << cache_size << endl;
        cout << "line_size  : " << cache_size << endl;
        cout << "n_way      : " << n_way << endl;
        cout << "num_sets   : " << num_sets << endl;
        */

        cout << std::hex;

    }

    void print_cache() {
        cout << "num_sets = " << std::dec << num_sets << endl;
        cout << "n_way    = " << n_way    << endl;
        for (u32 s = 0; s < num_sets; s++) {
            cout << "Set[" << s << "] {" << endl;
            for (u32 n = 0; n < n_way; n++) {
                cout << "    " << sets[s][n] << endl;
            }
            cout << "}" << endl;
        }
    }

    inline void print_statistics() {
        cout << cache_stats << endl;
    }

    u32 get_hits() {
        return cache_stats.cache_hits;
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
        if (fifo_ind) delete[] fifo_ind;
    }

    void request_addr(u32 addr) {
        assert(addr);
        CacheBlock * set;
        u32 index;
        find_in_cache(addr, set, index);
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
    inline void update_fifo(u32 set_num) {
        assert(set_num < num_sets);
        fifo_ind[set_num] = (fifo_ind[set_num] + 1) % n_way;
    }

    bool check_full() {
        if (isFull) return true;

        for (u32 s = 0; s < num_sets; ++s) {
            for (u32 n = 0; n < n_way; ++n) {
                if (!sets[s][n].written_to) return false;
            }
        }

        return isFull = true;
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

        index = 0;
        for (u32 p = 0; p < n_way; p++) {
            if ( (addr & tag_mask) == (set[p].tag & tag_mask) ) {
                index = p;
                set[index].age = 0;
                cache_stats.cache_hits++;
                return true;
            }

            if (set[p].age > set[index].age) {
                index = p;
            }
        }

        index = (LRU) ? index : fifo_ind[set_ind];
        assert(index < n_way);

        if (!set[index].written_to) {
            //cout << " compulsory miss " << endl;
            cache_stats.compulsory_misses++;
        } else if (check_full()) {
            //cout << " capacity miss " << endl;
            cache_stats.capacity_misses++;
        } else {
            //cout << " conflict miss " << endl;
            cache_stats.conflict_misses++;
        }

        // replace this block
        if (write_back && (set[index].dirty)) {
            write_block_to_memory(set + index, addr);
        }
        fetch_block_from_memory(set + index, addr);
        update_fifo(set_ind);

        return false;
    }


    /**
     * copies a block of memory from ram into this cache block
     * will update the tag bits, dirty bit and age for lru
     */
    void fetch_block_from_memory(CacheBlock *block, u32 addr) {
        assert(block != nullptr);

        block->age = block->dirty = 0; // clear block info
        block->tag = (addr & tag_mask);             // put new tag
        block->written_to = 1;
    }

    /**
     * copies a data in cache block into block of memory
     */
    void write_block_to_memory(CacheBlock *block, u32 addr) {
        assert(block != nullptr);
        block->dirty = 0;       // set dirty bit to zero
    }
};

#endif

