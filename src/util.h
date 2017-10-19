
#ifndef UTIL_H
#define UTIL_H

#include <string>
using namespace std;

namespace util {

inline bool isDigit(const char c);

char num2hex(int c);

int hex2num(char hexc);

int hex2i32(const string& hexnum);

string to_hex(int c);

} // end namespace util


namespace util {

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

int hex2i32(const string& hexnum) {
    int ret = 0;
    for (const char& c : hexnum) {
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

