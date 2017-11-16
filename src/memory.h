
#ifndef MEMORY_H
#define MEMORY_H

#include <iostream>
#include <assert.h>
#include "util.h"

struct Memory {
    u32 size;
    char * data;
    Memory(u32 size) {
        this->size = size;
        data = new char[size];
    }
    Memory(Memory && move) {
        this->size = move.size;
        this->data = move.data;
        move.data = nullptr;
    }
    ~Memory() {
        delete[] data;
    }

    char& operator [] (u32 addr) {
        assert(addr < size);
        return data[addr];
    }

};

#endif

