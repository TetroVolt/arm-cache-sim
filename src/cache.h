
#ifndef CACHE_H
#define CACHE_H

#include "util.h"
using namespace std;

struct CacheLine {
    u32 data_size = 0;
    u32 meta = 0; // meta data, contains tag data, remaining bits used for
                  // replacement policy
    char * data = nullptr;

    CacheLine() {}
    ~CacheLine() { if (data) delete[] data; }
};

class Cache {
protected:
    u32 num_groups; // number of associative groups
    u32 n_way;      // number of lines per group
    CacheLine ** groups; // 2D array memory

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
    Cache(u32 cache_size, u32 line_size, u32 n_way) {
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

    }

    ~Cache() {
        if (groups) {
            for (; --num_groups >= 0; delete[] groups[num_groups]);
            delete[] groups;
        }
    }

    inline void age_group(CacheLine * group) {
        for (u32 i = 0; i < n_way; ++i) {
            group[i].meta++;
        }
    }

    void store(u32 addr, char value) {
        //TO-DO
    }

    char load(u32 addr) {
        //TO-DO
        return 0;
    }
};

#endif

