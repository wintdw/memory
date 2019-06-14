# memory

# THP 
    #define MAPSIZE (long)1000*1024*1024*4
    ...
    madvise(ptr, MAPSIZE, MADV_HUGEPAGE);
## Result
    # perf stat -e dTLB-loads,dTLB-load-misses,dTLB-stores,dTLB-store-misses -- ./no_advise
    dTLB-load-misses          #    0.40% of all dTLB cache hits
    # perf stat -e dTLB-loads,dTLB-load-misses,dTLB-stores,dTLB-store-misses -- ./advise
    dTLB-load-misses          #    0.05% of all dTLB cache hits
