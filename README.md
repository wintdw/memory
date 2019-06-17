# MEMORY

### Table of Contents
* [Prerequisites](#prerequisites)<br>
* [Process Memory Layout](#process-memory-layout)<br>
  * [Stack and Heap](#stack--heap)<br>
* [Memory Mechanism](#memory-mechanism)<br>
  * [Out Of Memory (OOM)](#out-of-memory-oom)<br>
  * [Copy On Write (COW)](#copy-on-write-cow)<br>
  * [Overcommit](#overcommit)<br>
  * [Shared Memory - Inter-process Communication (IPC)](#share-memory---inter-process-communication-ipc)<br>
  * [Transparent Hugepage (THP)](#transparent-hugepage-thp)<br>

## Prerequisites
### C Functions
* memset
* malloc alloca
* mmap
* madvise MADV_HUGEPAGE
### Tools
* pmap
* perf
## Process Memory Layout
### Stack & Heap
```C
// Allocate 100m int objects on heap (x2)
alloc_heap(100000000);
alloc_heap(100000000);
```
```C
// Allocate 100m int objects on stack (x2)
alloc_stack(100000000);
alloc_stack(100000000);
```
#### Differences
```console
# Heap
Address           Kbytes     RSS   Dirty Mode    Mapping
00007f1562dd0000  781256  781256  781256 rw---   [ anon ]

781256 / 4 * 1024 = 200001536 ~ 2m int objects

# Stack
Address           Kbytes     RSS   Dirty Mode    Mapping
00007ffc51c26000  390636  390632  390632 rw---   [ stack ]

390636 / 4 * 1024 = 100002816 ~ 1m int objects
```
## Memory Mechanism
### Out Of Memory (OOM)
### Copy-On-Write (COW)
### Overcommit
### Shared Memory - Inter-Process Communication (IPC)
### Transparent Hugepage (THP)
#### 1. Simulations
```C
#define MAPSIZE 1024*1024*4     // 1m of int blocks
long size = (long)MAPSIZE * (long)atoi(argv[1]);
...
int *ptr = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
madvise(ptr, MAPSIZE, MADV_HUGEPAGE);
```
#### 2. Runtime metrics
```console
# cat /sys/kernel/mm/transparent_hugepage/enabled
always [madvise] never
# cat /proc/meminfo  | grep Anon
AnonPages:       7696872 kB
AnonHugePages:   4098048 kB
```
```console
# perf stat -e dTLB-load,dTLB-load-misses,cycles -- ./advise 2000

 Performance counter stats for './advise 2000':

        34,435,997      dTLB-load                                                   
            26,798      dTLB-load-misses          #    0.08% of all dTLB cache hits 
     5,151,598,257      cycles                   

       2.135126528 seconds time elapsed

# perf stat -e dTLB-load,dTLB-load-misses,cycles -- ./no_advise 2000

 Performance counter stats for './no_advise 2000':

     1,064,333,895      dTLB-load                                                   
         1,563,275      dTLB-load-misses          #    0.15% of all dTLB cache hits 
     8,797,338,489      cycles                   

       3.508520242 seconds time elapsed
```

#### 3. Not a silver bullet
  * Waste of memory sometimes
  * Slower because of compact, migration...
  Example: Allocation of 20G...
  ```C
# advise
+   98.68%     5.72%  advise  libc-2.19.so       [.] memset                                            
+   98.68%     0.00%  advise  libc-2.19.so       [.] __libc_start_main                                
+   51.29%    51.29%  advise  [kernel.kallsyms]  [k] isolate_migratepages_range                       
+   15.60%    15.60%  advise  [kernel.kallsyms]  [k] clear_page_c                                     
+    6.67%     6.67%  advise  [kernel.kallsyms]  [k] compaction_alloc                                 

# no_advise
+   89.51%    14.30%  no_advise  libc-2.19.so       [.] memset
+   89.51%     0.00%  no_advise  libc-2.19.so       [.] __libc_start_main
+   31.85%    31.85%  no_advise  [kernel.kallsyms]  [k] clear_page_c
+   12.07%    12.07%  no_advise  [kernel.kallsyms]  [k] page_fault
+    8.31%     8.31%  no_advise  [kernel.kallsyms]  [k] _raw_spin_lock
  ```
  ```console
# time ./advise 5000

real  0m22.884s
user  0m1.652s
sys	  0m21.244s

# time ./no_advise 5000 

real  0m8.520s
user  0m1.240s
sys   0m7.272s

# cat /proc/buddyinfo 
Node 0, zone   Normal  64522 171562 156286 108640  56285  17902   3005    322    121     59   1199 
Node 1, zone   Normal 253404 235857 186478  94067  28862   4424    463     66     45     39   2565 

Allocation without compaction: (2565 + 1199) * 4M = 14.7G
  ```

#### 5. Rerferences
https://alexandrnikitin.github.io/blog/transparent-hugepages-measuring-the-performance-impact/
