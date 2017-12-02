// imports
#include <iostream>
#include <fstream>
#include <cstring>
#include "cache.h"
#include "memory.h"
#include "util.h"
using namespace std;

// declarations
void print_usage(const char *);
int process_trace(istream& input);
void cache_math(u32 cache_size = 32 * 1024, u32 line_size = 32, u32 n_way = 2);
void cache_test();

// MAIN
int main(int argc, char ** argv) {

    int ret = 0;
    if (argc >= 2
        && (strcmp(argv[1], "-h") == 0
         || strcmp(argv[1], "--help") == 0)) {
        print_usage(argv[0]);
        goto end_prog;
    }

    if (argc == 1) {
        ret = process_trace(cin);
    } else if (argc ==3 && strcmp(argv[1], "-f") == 0) {
        fstream input;
        input.open(argv[2], ios::in);
        ret = process_trace(input);
        input.close();
    } else {
        cerr << "ERROR! malformed arguments." << endl;
        cerr << "use --help flag for usage" << endl;
        ret = -1;
    }

    //cache_math();
    //cache_test();

    end_prog: return ret;
}

int process_trace(istream& input) {
    u32 C, L, N, M;
    input >> C >> L >> N >> M;
    Cache cache(C, L, N, M);

    while (input.good()) {
        // read input
    }

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

void print_usage(const char * prognm) {
    const char* tabspace = "    ";
    cout
         << "Usage : " << prognm << " [flags] [arguments]" << endl << endl
         << "Behavior:" << endl
         << tabspace << "By default if no arguments are given the program" << endl
         << tabspace << "will read trace commands from standard input." << endl
         << tabspace << "The first 4 numbers of any input must be:" << endl
         << tabspace << " * Cache Size" << endl
         << tabspace << " * Line Size " << endl
         << tabspace << " * n_way association" << endl
         << tabspace << " * memory size" << endl
         << tabspace << "The program will keep reading input until end of file." << endl
         << tabspace << "commands take 2 forms:" << endl
         << tabspace << " * store:" << endl
         << tabspace << "     s 0x000f342b 0x11" << endl
         << tabspace << " * load :" << endl
         << tabspace << "      l 0x000f342b" << endl
         << tabspace << "The output of the program prints out loaded values," << endl
         << tabspace << "and cache statistics. " << endl << endl
         << "Flags:" << endl
         << tabspace << "   -h  --help      show this message" << endl
         << tabspace << "   -f <filenm>     specify tracefile instead of" << endl
         << tabspace << "                   reading from standard input" << endl << endl
         << "examples:" << endl
         << tabspace << prognm << " -h" << endl
         << tabspace << prognm << " -f ./tracefile.trace" << endl << endl
         ;
}
