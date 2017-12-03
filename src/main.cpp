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
int process_trace(fstream& input);

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

int process_trace(fstream& input) {
    u32 cache_size = 64 * 1024;
    u32 line_sizes[]  = {32, 64};
    u32 n_way[]       = {1, 2};
    u32 replace[]     = {0, 1};

    struct config {
        int line_size = -1;
        int association = -1;
        int replacement = -1;
        int hits = 0;
    };

    config configs[8];
    u32 requests;

    u32 g = 0;
    u32 max_hits = 0;
    for (u32 l = 0; l < 2; ++l)
    for (u32 r = 0; r < 2; ++r)
    for (u32 n = 0; n < 2; ++n)
    {
        input.clear();
        input.seekg(0, ios::beg);

        Cache cache(cache_size, line_sizes[l], n_way[n]);
        if (replace[r]) {
            //cout << "LRU replacement" << endl;
            cache.toggle_replacement();
        } else {
            //cout << "FIFO replacement" << endl;
        }

        u32 addr;
        requests = 0;
        while (input >> std::hex >> addr) {
            if (input.fail()) {
                cerr << "ERROR! invalid input" << endl;
                if (input.eof()) break;
                input.clear();
                input.ignore();
                continue;
            }

            cache.request_addr(addr);
            //cout << "Request at 0x" << std::hex << addr << endl;
            requests++;
        }

        //cache.print_cache();
        //cache.print_statistics();
        //cout << "Processed " << std::dec << requests << endl;

        configs[g].line_size = line_sizes[l];
        configs[g].association = n_way[n];
        configs[g].replacement = replace[r];
        configs[g].hits = cache.get_hits();
        if (configs[g].hits > max_hits) max_hits = configs[g].hits;
        g++;
    }

    done:
    cout << std::dec;
    cout << "BEST CONFIG(s) for " << requests << " requests : " << endl;
    cout << "max_hits: " << max_hits << endl;
    for (g = 0; g < 8; ++g) {
        if (configs[g].hits == max_hits) {
            cout << "config : " << endl;
            cout << "  line_size  : " << configs[g].line_size    << endl;
            cout << "  n_way      : " << configs[g].association  << endl;
            cout << "  replacement: ";
            if (configs[g].replacement) cout << "LRU"; else cout << "FIFO"; cout << endl;
            cout << "  hits       : " << configs[g].hits  << endl;
        }
    }

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
         << tabspace << "The program will keep reading input until end of file." << endl
         << tabspace << "Memory addresses must be in hex" << endl
         << tabspace << "The output of the program prints out loaded values," << endl
         << tabspace << "and cache statistics. " << endl << endl
         << "examples:" << endl
         << tabspace << prognm << " -h" << endl
         << tabspace << prognm << " tracefile.trace" << endl << endl
         ;
}
