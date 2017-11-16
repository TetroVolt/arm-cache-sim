
#ifndef CACHE_H
#define CACHE_H

#include "util.h"
#include "memory.h"
using namespace std;

struct CacheLine {
    u32 data_size = 0;
    u32 meta = 0;
        // this meta data contains meta data
        // [tag][age][dirty bit]
        // TO-DO implement proper masks and shifts
    char * data = nullptr;

    CacheLine() {}
    ~CacheLine() { if (data) delete[] data; }
};

class Cache {
protected:
    u32 num_groups; // number of associative groups
    u32 n_way;      // number of lines per group
    CacheLine ** groups; // 2D array memory
    Memory * ram;

    // helper masks and shifts
    u32 byte_mask;  // for index of individual byte per cache line in group
    u32 assoc_mask; // for index of group in groups
    u32 tag_mask;   // for tag comparison
    u32 assoc_shift;

public:
    /**
     * Constructor
     *  @param cache_size   size in bytes of the cache
     *  @param line_size    size of each cache line
     *  @param n_way        set associativity
     */
    explicit
    Cache(u32 cache_size, u32 line_size, u32 n_way, u32 ram_size) {
        if ( !util::is_pow2(cache_size)
             || !util::is_pow2(cache_size)
             || !util::is_pow2(n_way) ) {
            cerr << "ERROR! cache_size and line_size must be a power of 2!"
                 << endl;
            throw std::exception();
        }

        if (cache_size < (line_size * n_way)) {
            cerr << "Invalid ratio of parameters: "
                 << "cache_size : " << cache_size << endl
                 << "line_size  : " << line_size << endl
                 << "n_way      : " << n_way << endl;
            throw std::exception();
        }

        num_groups = cache_size / line_size / n_way;
        groups = new CacheLine*[num_groups];

        // initialize array
        for (u32 i = 0; i < num_groups; i++) {
            groups[i] = new CacheLine[n_way];
            for (u32 j = 0; j < n_way; j++) {
                groups[i][j].data_size = line_size;
                groups[i][j].data = new char[line_size];
            }
        }

        byte_mask   = line_size - 1;
        assoc_mask  = (num_groups - 1) << util::log2(line_size);
        assoc_shift = util::log2(line_size);
        tag_mask    = ~(byte_mask | assoc_mask);

        ram = new Memory(ram_size);
    }

    ~Cache() {
        if (groups) {
            for (; --num_groups >= 0; delete[] groups[num_groups]);
            delete[] groups;
        }
    }

    /** For LRU
     * ages every cacheline's age 
     */
    inline void age_group(CacheLine * group) {
        for (u32 i = 0; i < n_way; ++i) {
            group[i].meta++; // is wrong not correct
        }
    }

    /** find(@addr, @group, @index)
     *  returns: bool if addr is in this cache
     *
     *  behavior:
     *  if the @addr is contained in the cache, 
     *  this function sets @group to the group line
     *  and sets index to the index of the cache line
     *  in that group
     *
     *  if @addr is not in the cache, it sets index
     *  to the index of the cacheline which is oldest
     *  for LRU replacement
     *
     *  to access the cacheline -> group[index]
     */
    bool find(u32 addr, Cache * &group, u32 &index) {
        group = groups[(addr & assoc_mask) >> assoc_shift];
        u32 oldest = 0;
        for (u32 p, p < n_way; p++) {
            if (addr & tag_mask == group[p].meta & tag_mask) {
                index = p;
                return true;
            }

            // simultaneously find oldest index for lru
            if ( ((group[p].meta & (~tag_mask)) >> 1) >
                 ((group[oldest].meta & (~tag_mask)) >> 1)) {
                oldest = p;
            }
        }
        index = oldest;
        return false;
    }

    void store(u32 addr, char value) {
        //TO-DO
        CacheLine * group;
        u32 index;
        if (find(addr, group, index)) {
            // cache hit
            group[index][addr & byte_mask] = value;
            group[index].meta |= 1; // update dirty bit
        } else {
            // cache miss
            // have to write whats in cache into memory first
            // then copy proper block from memory at this address
            // into this cache, then write value into this cache line
            
            // test comment
        }
    }

    char load(u32 addr) {
        //TO-DO
        
        return 0;
    }

    void write() {
    
    }
};

#endif

