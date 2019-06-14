# MEMORY

## IV. Transparent Hugepage (THP)
### 1. Code
    ```C
    #define MAPSIZE (long)1000*1024*1024*4   // 4G
    ...
    int *ptr = mmap(NULL, MAPSIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    madvise(ptr, MAPSIZE, MADV_HUGEPAGE);
    ```
### 2. Runtime metrics
    ```
        # cat /proc/meminfo  | grep Anon
        AnonPages:       7696872 kB
        AnonHugePages:   4098048 kB
    ```

### 2. Perf results
    ```
    # perf stat -e dTLB-loads,dTLB-load-misses,dTLB-stores,dTLB-store-misses -- ./no_advise
    dTLB-load-misses          #    0.40% of all dTLB cache hits
    1.877448789 seconds time elapsed

    # perf stat -e dTLB-loads,dTLB-load-misses,dTLB-stores,dTLB-store-misses -- ./advise
    dTLB-load-misses          #    0.05% of all dTLB cache hits
    1.012778293 seconds time elapsed
    ```
