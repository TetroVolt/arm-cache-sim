
#ifndef MEMORY_H
#define MEMORY_H

class Memory {
public:
    size_t blk_size;
    char * data;
    unsigned int address;

    Memory(size_t size, int addr) {
        blk_size = size;
        data = new char[size];
        address = addr;
    }
    ~Memory() {
        delete[] data;
    }
};

class LRU_Table {
public:
    size_t size;
    Memory * mem;
    int * ages;

    LRU_Table(size_t table_size) {
        size = table_size;
        mem = new Memory[];
    }
    ~LRU_Table() {
        if (mem)  delete[] mem;
        if (ages) delete[] ages;
    }
};


#endif
