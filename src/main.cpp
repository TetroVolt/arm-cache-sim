// imports
#include <iostream>
#include <fstream>
#include "cache.h"
#include "memory.h"
#include "util.h"
using namespace std;

// declarations
void cache_math(u32 cache_size = 32 * 1024, u32 line_size = 32, u32 n_way = 2);
void cache_test();

// MAIN
int main(int argc, char ** argv) {
    //cache_math();
    cache_test();
    return 0;
}

void cache_test() {
    Cache cache(32, 8, 2, 128);
    u32 addr;
    u32 choice;
    while (cin.good()) {
        cout << "\n(1) Store\n(2) Load\n(3) Print Cache\n(4) quit" << endl;
        cout << "Enter choice: ";
        cin >> choice; cin.ignore();

        if (choice == 4) break;
        if (choice == 3) {
            cache.print_cache();
            continue;
        }

        cout << "Enter address: ";
        cin >> addr; cin.ignore();
        if (choice == 1) { // store
            cout << "Enter value to store: ";
            cin >> choice; cin.ignore();
            cache.store_byte(choice, addr);
        } else if (choice == 2) { // load
            cache.load_byte(choice, addr);
            cout << "Value loaded = " << choice << endl;
        }
    }
}

//Implementations
void cache_math(u32 cache_size, u32 line_size, u32 n_way) {

    u32 num_groups = cache_size / line_size;

    u32 byte_mask   = line_size/n_way - 1;
    u32 assoc_shift = util::log2(line_size / n_way);
    u32 assoc_mask  = (num_groups - 1) << assoc_shift;
    u32 tag_mask    = ~(byte_mask | assoc_mask);

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
