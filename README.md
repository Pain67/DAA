# DAA - Ducky's Arena Allocator
Region-based memory management


In computer science, region-based memory management is a type of memory management in which each allocated object is assigned to a region. A region, also called a partition, subpool, zone, arena, area, or memory context, is a collection of allocated objects that can be efficiently reallocated or deallocated all at once. Memory allocators using region-based managements are often called area allocators, and when they work by only "bumping" a single pointer, as bump allocators.


## Concept

DAA defines two different type of areana allocators LinearArena and SmartArena. They both follow the basic concepts of region based allocation. With the first allocation, a region of memory is allocated (region size can be defined by #define). Any subsequent allocation will return pointer to that allocated region until it runs out. At that point another region will be allocated and cycle continues. The two allocator differ on how they manage each regions:

### LinearArena
You may be surprised but it allocates memory lineary. With every new allocation it will be "appended" at the end of the last one. This makes it much faster as there is no check it just return the next byte after the last allocation and marks its size to move the "position" of the next allocation after it. The downsize of this is, in this Arena there is no way to deallocate. Only the entire arena can be deallocated at once.

```c++
Example (each character represents 1 byte - 16 bytes per region)

When region created its empty
  ................
  ^

Allocate 4 bytes:
  ####............
      ^

Allocate 8 bytes:
  ############....
              ^
Reset Arena:
  ############....
  ^
Note: When region reset, it will not clear (zero) the memory,
just moves the position of the next allocation back to the begining
```

### SmartArena

Smart Arena keeps track of each allocation. This will give the option to deallocate individual allocations but comes with a overhead. When a new allocation is requested, the process now have to check each regions and each of their allocation to find the first unused allocatable space with size >= of the request. This means it has a linear complexity. This kinda works same as partitioning:

```c++
Example (each character represents 1 byte - 16 bytes per region)

When region is created it contains only one allocations:
  ................
Allocations: [0] Offset: 0, Size: 16, FREE


Allocate 4 bytes:
  ####............
Allocations: [0] Offset: 0, Size: 4, USED
             [1] Offset: 4, Size: 12, FREE

Allocate 4 bytes:
  ########........
Allocations: [0] Offset: 0, Size: 4, USED
             [1] Offset: 4, Size: 4, USED
             [2] Offset: 8, Size: 8, FREE
Allocate 4 bytes:
  ############....
Allocations: [0] Offset: 0, Size: 4, USED
             [1] Offset: 4, Size: 4, USED
             [2] Offset: 8, Size: 4, USED
             [3] Offset: 12, Size: 4, FREE

Free the 2nd Allocation (Index 1):
  ####....####....
Allocations: [0] Offset: 0, Size: 4, USED
             [1] Offset: 4, Size: 4, FREE
             [2] Offset: 8, Size: 4, USED
             [3] Offset: 12, Size: 4, FREE

Allocate 2 bytes
  ######..####....
Allocations: [0] Offset: 0, Size: 4, USED
             [1] Offset: 4, Size: 2, USED
             [2] Offset: 6, Size: 2, FREE
             [3] Offset: 8, Size: 4, USED
             [4] Offset: 12, Size: 4, FREE

Free the 3rd allocation (Index 3)
  ######..........
Allocations: [0] Offset: 0, Size: 4, USED
             [1] Offset: 4, Size: 2, USED
             [2] Offset: 6, Size: 10, FREE

Note: When allocation freed, it will check before and after to see if it can be merged.

```

## Benchmark

Execution time measured using "time.h":

```c++
clock_t Begin = clock();
...
clock_t End = clock();
double Time = (double)(End - Begin) / CLOCKS_PER_SEC;
```

There are 5 different types of test completed:
 - Allocate random size of memory (between 1 and 128 bytes)
 - Allocate Fixed size (1 byte) memory
 - Allocate Fixed size (2 byte) memory
 - Allocate Fixed size (4 byte) memory
 - Allocate Fixed size (8 byte) memory

Conditions:
 + Page Size set to 4096 bytes
 + For each test there are 100,000 allocations repeated 100 times and get the Total, Avg, Min and Max time. Malloc used as a base line to see how the 2 different arena compares to it
 + For the Random number allocation, a list of 100,000 random number generated ahead of time so all 3 uses the same list of random numbers.
 + The tests below only test for allocation and not free.


### Random Size Allocation (1 - 128 bytes) - 100000 allocations - 100 rounds per allocator
| Allocator | Total Time (sec) | AvgTime (sec) | Min (sec) | Max (sec) |
| --------- |:----------------:|:-------------:|:---------:|:---------:|
| malloc | 0.387733 | 0.003877 | 0.003671 | 0.004388 |
| LinearArena | 0.058677 | 0.000587 | 0.000557 | 0.002872 |
| SmartArena | 49.540262 | 0.495403 | 0.449762 | 0.553725 |

### Fixed Size Allocation (1 bytes) - 100000 allocations - 100 rounds per allocator
| Allocator | Total Time (sec) | AvgTime (sec) | Min (sec) | Max (sec) |
| --------- |:----------------:|:-------------:|:---------:|:---------:|
| malloc | 0.208735 | 0.002087 | 0.001024 | 0.003160 |
| LinearArena | 0.051953 | 0.000520 | 0.000495 | 0.000929 |
| SmartArena | 30.697113 | 0.306971 | 0.277660 | 0.335800 |

### Fixed Size Allocation (2 bytes) - 100000 allocations - 100 rounds per allocator
| Allocator | Total Time (sec) | AvgTime (sec) | Min (sec) | Max (sec) |
| --------- |:----------------:|:-------------:|:---------:|:---------:|
| malloc | 0.184611 | 0.001846 | 0.001164 | 0.003140 |
| LinearArena | 0.045765 | 0.000458 | 0.000437 | 0.000681 |
| SmartArena | 15.934915 | 0.159349 | 0.148214 | 0.171679 |

### Fixed Size Allocation (4 bytes) - 100000 allocations - 100 rounds per allocator
| Allocator | Total Time (sec) | AvgTime (sec) | Min (sec) | Max (sec) |
| --------- |:----------------:|:-------------:|:---------:|:---------:|
| malloc | 0.206957 | 0.002070 | 0.001339 | 0.003581 |
| LinearArena | 0.051750 | 0.000518 | 0.000500 | 0.000746 |
| SmartArena | 8.656300 | 0.086563 | 0.082830 | 0.097035 |

### Fixed Size Allocation (8 bytes) - 100000 allocations - 100 rounds per allocator
| Allocator | Total Time (sec) | AvgTime (sec) | Min (sec) | Max (sec) |
| --------- |:----------------:|:-------------:|:---------:|:---------:|
| malloc | 0.183952 | 0.001840 | 0.001103 | 0.003318 |
| LinearArena | 0.048748 | 0.000487 | 0.000443 | 0.000675 |
| SmartArena | 5.962252 | 0.059623 | 0.056750 | 0.073468 |
