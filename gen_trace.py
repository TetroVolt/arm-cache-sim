#!/usr/bin/env python3

def generate(filenm, ram_size, operations):
    import random
    ops = ['s', 'l']

    with open(filenm, 'w') as f:
        for i in range(operations):
            if random.randint(0,1):
                st = 's {} {}'.format('{0:#0{1}x}'.format(random.randint(0, ram_size-1), 10),
                                      '{0:#0{1}x}'.format(random.randint(0, 255), 4))
            else:
                st = 'l {}'.format('{0:#0{1}x}'.format(random.randint(0, ram_size-1), 10))
            print(st)
            f.write(st + '\n')


def Main():
    generate('tracefile.txt', 128 * 1024, 20)
    pass

if __name__ == '__main__':
    Main()
