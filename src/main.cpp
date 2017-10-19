
#include <iostream>
#include "util.h"
using namespace std;

int main(int argc, char ** argv) {
    string my_hex = "0x0000000a";
    int c = util::hex2i32(my_hex);
    cout << c << endl;
    return 0;
}



