# arm-cache-sim
Arm Cache Simulator for Computer Organization and Architecture

Memory Size used: 1 MiB
Cache Size used : 64 KiB

gen\_trace.py is used to generate the tracefiles

trace1.py are requests for 128  KiB memory
trace2.py are requests for 256  KiB memory
trace3.py are requests for 512  KiB memory
trace4.py are requests for 1024 KiB memory
trace5.py are requests for 2048 KiB memory

conclusions:
    replacement scheme does not matter
    n_way = 1, or direct mapping seems to be the best

Original Requirements:
  * note:
    - discarded s, l, and cycle requirements for trace files
    - discarded write policy since it does not affect hit ratio
```
+-----------------------------------------------------------+
|                      #  Trace Files #                     |
+-----------------------------------------------------------+
|  The trace files contain three types of information:      |
|  1. Access type:                                          |
|      ‘l’ represents ‘load’ operation;                     |
|      ‘s’ represents ‘store’ operation.                    |
|                                                           |
|  2. Address: Using 32 bits unsigned hexadecimal;          |
|                                                           |
|  3. Number of cycles: which presents how many cycles      |
|     have been passed after last access.                   |
+-----------------------------------------------------------+

+-----------------------------------------------------------+
|                      # Requirements #                     |
+-----------------------------------------------------------+
|  1. Please count the instruction type of the Load type    |
|     instruction and the Store type instruction in the     |
|     five trace files.                                     |
|                                                           |
|  2. Set the total capacity of cache is 32KB.              |
|     Then combine all the following parameters to test     |
|     cache hit ratio of the 5 trace files.                 |
|                                                           |
|      * Line size             : a) 32 byte;   b) 64 byte   |
|      * Associativity         : a) 1-way;     b) 2-way     |
|      * Replacement policies  : a) FIFO;      b) LRU       |
|      * Write policies        : a) write through;          |
|                                b) write back              |
|                                                           |
|  3. Provide the best combination for five files by        |
|     comparing cache hit rate. For different trace file,   |
|     is the best configuration always the same?            |
+-----------------------------------------------------------+
```
