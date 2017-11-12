# arm-cache-sim
Arm Cache Simulator for Computer Organization and Architecture

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

