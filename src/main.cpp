// imports
#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include "cache.h"
#include "memory.h"
#include "util.h"
using namespace std;

// function declarations
void print_usage(const char *);
int process_trace(istream& input);

// MAIN
int main(int argc, char ** argv) {
    if (argc == 1 || (argc >= 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0))) {
        print_usage(argv[0]);
        return argc != 2;
    }

    fstream input;
    input.open(argv[1], ios::in);
    if (!input.is_open()) {
        cerr << "ERROR! cannot open file " << argv[1] << endl;
        return -1;
    }
    int ret = process_trace(input);
    input.close();

    return ret;
}

int process_trace(istream& input) {
    u32 C, L, N, M, requests = 0;
    input >> std::dec >> C >> L >> N >> M >> std::hex;

    cout << "Cache  Size: " << C << endl;
    cout << "Line   Size: " << L << endl;
    cout << "N_way  Size: " << N << endl;
    cout << "Memory Size: " << M << endl;

    cout << std::hex;
    Cache cache(C, L, N, M);

    string ins;
    while (input >> ins) {
        requests++;
        if (ins == "s") { // perform store
            input >> L >> C;
            if (!input.good()) goto fixerr;

            cache.store_byte(C, L);
            cout << "Stored at addr 0x" << L << " {data: 0x" << C << "}" << endl;

        } else if (ins == "l") { // perform load
            input >> L;
            if (!input.good()) goto fixerr;

            cache.load_byte(C, L);
            cout << "Loaded from addr 0x" << L << " {data: 0x" << C << "}" << endl;
        } else if (ins == "print") {
            cache.print_cache();
        } else {
            cerr << "ERROR! Unknown instruction \"" << ins << "\"" << endl;
            requests--;
        }

        continue;

        fixerr:
            cerr << "ERROR! invalid input" << endl;
            if (input.eof()) break;
            input.clear();
            input.ignore();

    }

    cache.print_cache();
    cache.print_statistics();

    cout << "Completed " << std::dec << requests << " requests." << endl;

    return 0;
}


void print_usage(const char * prognm) {
    const char* tabspace = "    ";
    cout
         << "Usage : " << prognm << " <tracefilenm>" << endl << endl
         << "Behavior:" << endl
         << tabspace << "This program calculates the best cache configuration from" << endl
         << tabspace << "cache_size  = 32 kb , 64 kb" << endl
         << tabspace << "n_way       = 1 way, 2 way" << endl
         << tabspace << "replacement = LRU, or FIFO" << endl
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
         << "examples:" << endl
         << tabspace << prognm << " -h" << endl
         << tabspace << prognm << " tracefile.trace" << endl << endl
         ;
}
