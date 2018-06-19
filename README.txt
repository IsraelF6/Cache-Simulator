===================
  Cache Simulator
===================

Made By: Israel Felhandler


I. Purpose:
The purpose of this project is to exercise your understanding of caches of various sizes and configurations, 
as well as the write policies of write-through and write-back.

II. Basic Description:
You will be implementing a cache simulator in C, which will be contained in the single file proj3.c. 
Your program should take as command line arguments the following information:
- b: size of block in bytes.
- s: number of sets in the cache.
- n: associativity of cache.

Your submitted proj3.c must compile and run on linprog as in the following example:
 $ gcc –lm proj3.c
 $ ./a.out –b 8 –s 256 –n 2 < test1.trace

The contents of the .trace files used as input are described below.
After reading in the command line arguments, you should print back to the user the number of
bytes in a block, the number of sets in the cache, and the associativity of the cache.
Afterward, you should prepare to simulate the behavior of two caches on the sequence of
references in the trace file. The first cache has a write-through, no write allocate policy. The
second has a write-back, write allocate policy. For every reference, you should determine for 
each cache whether the reference is a hit or a miss, and whether main memory needs to be
accessed. After all references have been processed, print the statistics for each cache.

For example, consider the contents of the trace file test.trace:
W 300
R 304
R 4404
W 4408
W 8496
R 8500
R 304

Let’s use this trace file as input to our program and specify that our caches should have 64 sets
with 2 blocks each and 16 bytes per block.

$ gcc -lm proj3.c
$ ./a.out -b 16 -s 64 -n 2 < test.trace
Block size: 16
Number of sets: 64
Associativity: 2
Number of offset bits: 4
Number of index bits: 6
Number of tag bits: 22
****************************************
Write-through with No Write Allocate
****************************************
Total number of references: 7
Hits: 1
Misses: 6
Memory References: 7
****************************************
Write-back with Write Allocate
****************************************
Total number of references: 7
Hits: 2
Misses: 5
Memory References: 6

III. Trace File Description:
The .trace files that will be used as input to your cache simulator contain any number of lines,
where each line has the following format:

 <access_type> <byte_address>

For a write-through, no write allocate cache, we have the following properties:
- When a block is present in the cache (hit), a read simply grabs the data for the processor.
- When a block is present in the cache (hit), a write will update both the cache and main
memory (i.e. we are writing through to main memory).
- When a block is not present in the cache (miss), a read will cause the block to be pulled from
main memory into the cache, replacing the least recently used block if necessary.
- When a block is not present in the cache (miss), a write will update the block in main memory
but we do not bring the block into the cache (this is why it is called “no write allocate”).

After you have implemented the write-through cache, you should implement the write-back cache. One way to 
approach this is to have two separate caches, one for write-through and the other for write-back, which are 
both updated independently every time a reference is read from the trace file. 

For a write-back, write allocate cache, we have the following properties:
- When a block is present in the cache (hit), a read simply grabs the data for the processor.
- When a block is present in the cache (hit), a write will update only the cache block and set the dirty bit 
  for the block. The dirty bit indicates that the cache entry is not in sync with main memory and will need 
  to be written back to main memory when the block is evicted from the cache.
- When a block is not present in the cache (miss), a read will cause the block to be pulled from main memory 
  into the cache, replacing the least recently used block if necessary. If the block being evicted is dirty, 
  the block’s contents must be written back to main memory.
- When a block is not present in the cache (miss), a write will cause the block to be pulled from main memory 
  into the cache, replacing the least recently used block if necessary. When the block is pulled into the cache, 
  it should immediately be marked as “dirty”. If the block being evicted is dirty, the block’s contents must be
  written back to main memory.

Using these properties, you must calculate the number of hits and misses as well as the
memory references made by the sequence of addresses in the trace file. A memory reference is
defined as an access to main memory in order to either update or read a block. You do not
need to model the data or main memory, we are just simulating the effect of the references,
not dealing with actual data.
