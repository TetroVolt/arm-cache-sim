// imports
#include <iostream>
#include "cache.h"
#include "memory.h"
#include "util.h"
using namespace std;

// declarations
void cache_math(u32 cache_size, u32 line_size, u32 n_way);
void cache_test();

// MAIN
int main(int argc, char ** argv) {
    cache_test();
    return 0;
}

void cache_test() {
    Cache cache(32, 4, 2, 64);
    cache.print_cache();
}

//Implementations
void cache_math(u32 cache_size, u32 line_size, u32 n_way) {
    //u32 cache_size = 32 * 1024;
    //u32 line_size = 32;
    //u32 n_way = 1;

    u32 num_groups = cache_size / n_way / line_size;

    u32 byte_mask   = line_size - 1;
    u32 assoc_mask  = (num_groups - 1) << util::log2(line_size);
    u32 tag_mask    = ~(byte_mask | assoc_mask);
    u32 assoc_shift = util::log2(line_size);

    cout << "line_size   = " << line_size << endl;
    cout << "n_way       = " << n_way << endl;
    cout << "cache_size  = " << cache_size << endl;
    cout << "num_groups  = " << num_groups << endl;
    cout << "assoc_shift = " << assoc_shift << endl;

    cout << "Binary byte_mask  : ";
    util::print_bin(byte_mask);
    cout << "Binary assoc_mask : ";
    util::print_bin(assoc_mask);
    cout << "Binary tag_mask   : ";
    util::print_bin(tag_mask);

    cout << "a_mask >> a_shift : ";
    util::print_bin(assoc_mask >> assoc_shift);
}
