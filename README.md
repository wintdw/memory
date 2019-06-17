# MEMORY

### Table of Contents
**[Process Memory Layout](#process-memory-layout)**<br>
  **[Stack and Heap](#stack--heap)**<br>
**[Memory Mechanism](#memory-mechanism)**<br>
  **[Out Of Memory (OOM)](#out-of-memory-oom)**<br>
  **[Copy On Write (COW)](#copy-on-write-cow)**<br>
  **[Overcommit](#overcommit)**<br>
  **[Shared Memory - Inter-process Communication (IPC)](#share-memory---inter-process-communication-ipc)**<br>
  **[Transparent Hugepage (THP)](#transparent-hugepage-thp)**<br>

## Process Memory Layout
### Stack & Heap
## Memory Mechanism
### Out Of Memory (OOM)
### Copy-On-Write (COW)
### Overcommit
### Shared Memory - Inter-Process Communication (IPC)
### Transparent Hugepage (THP)
### 1. Code
```C
#define MAPSIZE (long)2000*1024*1024*4   // 4G
...
int *ptr = mmap(NULL, MAPSIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
madvise(ptr, MAPSIZE, MADV_HUGEPAGE);
```
### 2. Runtime metrics
```console
# cat /sys/kernel/mm/transparent_hugepage/enabled
always [madvise] never
# cat /proc/meminfo  | grep Anon
AnonPages:       7696872 kB
AnonHugePages:   4098048 kB
```
### 3. Perf results
```console
# perf stat -e dTLB-load,dTLB-load-misses,cycles -- ./no_advise 

 Performance counter stats for './no_advise':

     1,065,214,287      dTLB-load                                                   
         2,554,625      dTLB-load-misses          #    0.24% of all dTLB cache hits 
     8,993,703,771      cycles                   

       3.583700400 seconds time elapsed

# perf stat -e dTLB-load,dTLB-load-misses,cycles -- ./advise 

 Performance counter stats for './advise':

        33,318,493      dTLB-load                                                   
            12,788      dTLB-load-misses          #    0.04% of all dTLB cache hits 
     4,844,397,465      cycles                   

       2.018800299 seconds time elapsed
```
### 4. Not a silver bullet
  * Waste of memory sometimes
  * Slower because of compact, reclaimation...
  ```C
# advise
+   98.68%     5.72%  advise  libc-2.19.so       [.] memset                                             ◆
+   98.68%     0.00%  advise  libc-2.19.so       [.] __libc_start_main                                  ▒
+   51.29%    51.29%  advise  [kernel.kallsyms]  [k] isolate_migratepages_range                         ▒
+   15.60%    15.60%  advise  [kernel.kallsyms]  [k] clear_page_c                                       ▒
+    6.67%     6.67%  advise  [kernel.kallsyms]  [k] compaction_alloc                                   ▒

# no_advise
+   89.51%    14.30%  no_advise  libc-2.19.so       [.] memset
+   89.51%     0.00%  no_advise  libc-2.19.so       [.] __libc_start_main
+   31.85%    31.85%  no_advise  [kernel.kallsyms]  [k] clear_page_c
+   12.07%    12.07%  no_advise  [kernel.kallsyms]  [k] page_fault
+    8.31%     8.31%  no_advise  [kernel.kallsyms]  [k] _raw_spin_lock
  ```
  ```console
# time ./advise 5000

real	0m22.884s
user	0m1.652s
sys	0m21.244s

# time ./no_advise 5000

real	0m8.520s
user	0m1.240s
sys	0m7.272s

  ```

### 5. Rerferences
https://alexandrnikitin.github.io/blog/transparent-hugepages-measuring-the-performance-impact/
