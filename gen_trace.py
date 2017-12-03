#!/usr/bin/env python3

def generate(filenm, ram_size, operations):
    import random
    ops = ['s', 'l']

    with open(filenm, 'w') as f:
        for i in range(operations):
            st = '{}'.format('{0:#0{1}x}'.format(random.randint(1, ram_size-1), 10))
            print(st)
            f.write(st + '\n')

def Main():
    generate('trace1.trace', 128 * 1024, 1024)
    generate('trace2.trace', 256 * 1024, 1024)
    generate('trace3.trace', 512 * 1024, 1024)
    generate('trace4.trace', 1024 * 1024, 1024)
    generate('trace5.trace', 2048 * 1024, 1024)
    pass

if __name__ == '__main__':
    Main()
