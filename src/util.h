
#ifndef UTIL_H
#define UTIL_H

#include <cstring>
#include <string>
using namespace std;

#define u32 unsigned int
#define u8 unsigned char

// min max macros
#define MAX(a,b) ((a > b) ? a : b)
#define MIN(a,b) ((a < b) ? a : b)


namespace util {

    inline bool is_pow2(unsigned int x) {
        return (x!=0) && !(x & (x-1));
    }

    inline u32 log2(u32 x) {
        u32 ret = 0;
        while (x >>= 1) ret++;
        return ret;
    }

    inline bool isDigit(const char c) {
        return '0' <= c && c <= '9';
    }

    // meant for primitives
    template<typename T>
    void print_bin(T t) {
        unsigned long u64 = 0x00;
        memcpy(&u64, &t, sizeof(T));
        char buff[sizeof(T) * 8 + 1];
        buff[sizeof(T) * 8] = '\0';
        for (size_t bit = 0; bit < sizeof(T) * 8; bit++) {
            buff[sizeof(T) * 8 - 1 - bit] = '0' + (u64 & 1);
            u64 >>= 1;
        }
        //cout << buff;
        for (size_t byte = 0; byte < sizeof(T); byte++) {
            for (char bit = 0; bit < 8; bit++) {
                cout << buff[byte*8 + bit];
            }
            if ((byte < sizeof(T) - 1)) cout << " ";
        }
        //cout << endl;
    }


    char num2hex(int c) {
        if (0 <= c && c <= 9) return c + '0';

        switch (c) {
            case 10:
                return 'a';
            case 11:
                return 'b';
            case 12:
                return 'c';
            case 13:
                return 'd';
            case 14:
                return 'e';
            case 15:
                return 'f';
        }

        return '\0';
    }

    inline char to_lower(char c) {
        return ('A' <= c && c <= 'Z') ? c + 'a' - 'A' : c;
    }

    unsigned int hex2num(char c) {
        if (isDigit(c)) return c - '0';
        c = to_lower(c);
        switch (c) {
            case 'a':
                return 10;
            case 'b':
                return 11;
            case 'c':
                return 12;
            case 'd':
                return 13;
            case 'e':
                return 14;
            case 'f':
                return 15;
        }

        return -1;
    }

    /**
     *  hexnum must be a 32 hex number
     */
    unsigned int hex2u32(const string& hexnum) {
        unsigned int ret = 0;
        for (const char &c : hexnum) {
            ret = ret << 4;
            ret |= hex2num(c);
        }
        return ret;
    }

    string to_hex(int c) {
        string ret = "0x00000000";
        int i = ret.length() - 1;
        while (i > 1) {
            ret[i] = num2hex(c & 0xf);
            c >> 4;
            i--;
        }
        return ret;
    }

} // end namespace util

#endif

