# MEMORY
[[_TOC_]]
## Process Memory Layout
### Stack & Heap
## Mechanism
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
  * Slower in some allocations
  * Waste of memory sometimes
  * Spikes because of khugepaged (compact, reclaim...)

### 5. Rerferences
https://alexandrnikitin.github.io/blog/transparent-hugepages-measuring-the-performance-impact/
