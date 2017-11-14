
#ifndef UTIL_H
#define UTIL_H

#include <string>
using namespace std;

namespace util {

// returns true if c is between '0' and '9'
inline bool isDigit(const char c);

// converts a number from 0-15 to hex char
char num2hex(int c);

// converts a hex char into an integer
int hex2num(char hexc);

// converts a hex string into an integer
int hex2i32(const string& hexnum);

// converts an int into a hex string
string to_hex(int c);

bool is_pow2(unsigned int x);

} // end namespace util



// implementations

namespace util {

inline bool is_pow2(unsigned int x) {
    return (x!=0) && !(x & (x-1));
}

inline bool isDigit(const char c) {
    return '0' <= c && c <= '9';
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

int hex2num(char c) {
    if (isDigit(c)) return c - '0';
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
int hex2i32(const string& hexnum) {
    int ret = 0;
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

