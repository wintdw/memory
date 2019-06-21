# MEMORY

### Table of Contents
* [Prerequisites](#prerequisites)<br>
* [Process Memory Layout](#process-memory-layout)<br>
  * [Stack and Heap](#stack--heap)<br>
* [Memory Mechanism](#memory-mechanism)<br>
  * [Overcommitment](#overcommitment)<br>
  * [Out Of Memory (OOM)](#out-of-memory-oom)<br>
  * [Shared Memory - Inter-process Communication (IPC)](#share-memory---inter-process-communication-ipc)<br>
  * [Copy On Write (COW)](#copy-on-write-cow)<br>
  * [Transparent Hugepage (THP)](#transparent-hugepage-thp)<br>

## Prerequisites
### C Functions
* memset: set the block memory to the specific value
* fork: create a new process, copy the same memory layout of the parent
* malloc: allocate memory region from the heap
* alloca: allocate memory region from the stack
* mmap: creates a new memory mapping in the calling process’s virtual address space
  * MAP_PRIVATE: modifications to the contents of the mapping are not visible to other processes
  * MAP_SHARED: modifications to the contents of the mapping are visible to other processes that share the same mapping
  * MAP_NORESERVE: allow overcommits
* madvise: allows a process to advise the kernel about its future patterns of usage of a virtual memory region
  * MADV_HUGEPAGE: enables Transparent Huge Pages (THP) for pages
### Tools
* pmap: report memory map of a process. Interprets from /proc/.../maps & /proc/.../smaps
* perf: performance analysis tools
## Process Memory Layout
### Layout
![process memory layout](https://github.com/wintdw/memory/blob/master/proc_mem_layout.png "Process Memory Layout")
* Virtual memory size is nearly infinite (256TB for 64bit machine)
* Stack grows downward, Heap grows upward
* Stack: automatic allocation & deallocation
* Heap: dynamic & manual, much larger than stack
### Stack & Heap
#### Simulation
```C
#define ONEG 1024*1024*1024l

// Allocate 1G on heap (x2)
alloc_heap(ONEG);
alloc_heap(ONEG);

// Allocate 1G on stack (x2)
alloc_stack(ONEG);
alloc_stack(ONEG);
```
```console
# gcc heapstack/heap.c -o heap
# gcc heapstack/stack.c -o stack 
# ./heap & ./stack &
```
#### Difference
```console
# Heap
Address           Kbytes     RSS   Dirty Mode    Mapping
00007f5a61180000 2097160 2097160 2097160 rw---   [ anon ]

# Stack
Address           Kbytes     RSS   Dirty Mode    Mapping
00007ffc39367000 1048584 1048584 1048584 rw---   [ stack ]
```
#### Explanation
* By default, stack size for a process is limited to 8192KB, while heap doesnt have any limit
  ```console
  stack size              (kbytes, -s) 8192
  ```
* Primitive values are stored inside stack
* Stack memory will be recycled when function returns. Stack can not be leaked
* Heap memory can be leaked
* Stack allocation is faster than the counterpart's
## Memory Mechanisms
### Overcommitment
#### Definition
The assignment of more memory to a process virtual memory than the physical memory that are actually hosted. This is possible because a processs doesnt necesarily use all the memory it's assigned.
#### Simulation
```C
#define MAPSIZE 1024*1024*1024l
...
int *ptr = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_NORESERVE, -1, 0);
memset(ptr, 0, size/4);
```
```console
# gcc overcommitment/overcommitment.c -o overcommitment
# ./overcommitment 6 & overcommitment 6 &   # allocate 6G x2 for each proc
```
#### Result
```console
Address           Kbytes     RSS   Dirty Mode    Mapping
00002aeede82b000 6291460 1572868 1572868 rw---   [ anon ]

# cat /proc/meminfo
Committed_AS:   13075236 kB   // 13G
```
#### Explanation
* Using ```malloc()``` and ```mmap()``` is declaration of memory allocation, and the kernel doesnt actually allocate physical memory for it. ```memset()``` writes to the region, so that the memory is really allocated
* Why overcommitment? <br>
  Some applications create large (usually private anonymous) mappings, but use only a small part of the mapped region. For example, certain types of scientific applications allocate a very large array, but operate on only a few widely separated elements of the array (a so-called sparse array).
* Settings <br>
  /proc/sys/vm/overcommit_memory <br>
  
  | Value | MAP_NORESERVE     |  Default                |
  |-------|-------------------|-------------------------|
  | 0 (default)    | Allow Overcommit  | Deny Obvious Overcommit |
  | 1     | Allow Overcommit  | Allow Overcommit        |
  | 2     | Strict Overcommit | Strict Overcommit       |

### Out Of Memory (OOM)
#### Definition
No available memory can be allocated for use to processes. For this situation, kernel triggers ```OOM Killer``` to stop a hogging process for others to run properly.
#### Simulation
Same as Overcommit experiment, just spawn more processes until something is killed
#### Result
```console
# ./important & ./not_so_imp &
20446 root      20   0 6295496 1.501g    936 S   0.0 38.8   0:04.60 important                           
20447 root      20   0 6295496 1.201g    776 S   0.0 31.1   0:03.74 not_so_imp 

# ./not_so_imp & 
20573 root      20   0 6295496 1.201g    860 S   0.0 31.1   0:02.85 not_so_imp                          
20447 root      20   0 6295496 1.201g    772 S   0.0 31.1   0:03.74 not_so_imp

important is killed by OOM Killer
```
```console
# echo -1000 > /proc/20847/oom_score_adj

# ./not_so_imp &
20847 root      20   0 6295496 1.501g    848 S   0.0 38.8   0:04.40 important                           
21018 root      20   0 6295496 1.201g    920 S   0.0 31.1   0:02.07 not_so_imp

not_so_imp is killed by OOM Killer
```
#### Conclusion
* Adjust oom_score_adj to advoid critical services being killed
* Trigger OOM killer manually
```console
# echo f > /proc/sysrq-trigger 
```
### Shared Memory - Inter-Process Communication (IPC)
#### Definition
The memory region that may be simultaneously accessed by multiple programs with an intent to provide communication among them or avoid redundant copies
![shared memory](https://github.com/wintdw/memory/blob/master/sharedmem.gif "Shared memory")
#### Simulation
* Shared anonymous mapping 
```C
// Shared region for both parent and child
int *ptr = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
memset(ptr, 1, MAPSIZE);

pid_t child;
if ((child = fork()) == 0) {
    memset(ptr, 2, size);
    // Map another private region on child
    int *p = mmap(NULL, ONEG, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    memset(p, 3, ONEG);
```
```console
# ./sharemem 2 &

  PID USER      PR  NI    VIRT    RES    SHR S  %CPU %MEM     TIME+ COMMAND                             
  505 root      20   0 3149768 3.000g 2.000g S   0.0 77.7   0:04.02 sharemem
  477 root      20   0 2101192 2.001g 2.001g S   0.0 51.8   0:05.60 sharemem

# cat /proc/meminfo
Mapped:          2137732 kB
Shmem:           2139348 kB     // 2G
```
* /dev/shm confusion
```console
# dd if=/dev/zero of=big bs=2M count=1000
# ./mem 2   # try allocating 2G of mem to a process
Segmentation Fault
# cat /proc/meminfo
MemAvailable:    1632024 kB
Shmem:           2066996 kB     // 2G
```
#### Explanation
* Shared memory region is mapped only one on physical memory
* /dev/shm is also counted as Shared Memory, and can not be automatically evicted
### Copy-On-Write (COW)
#### Definition
A resource-management technique which is used to efficiently implement a "duplicate" or "copy" operation on modifiable resources. If a resource is duplicated but not modified, it is not necessary to create a new resource; the resource can be shared between the copy and the original. Modifications must still create a copy, hence the technique: the copy operation is deferred to the first write
#### Simulation
```C
// Create a memory region for all the processes, private
int *array = malloc(size);
memset(array, 0, size);

// Create 2^3 = 8 procs with the same private region
pid_t child;
if ((child = fork()) == 0) {
    int devNull = open("/dev/null", O_WRONLY|O_CREAT|O_APPEND);
    int b = write(devNull, array, size);
}
x3
```
```console
# gcc copyonwrite.c -o copyonwrite
# ./copyonwrite 2       # 2G for each process
```
#### Result
```console
  PID USER      PR  NI    VIRT    RES    SHR S  %CPU %MEM     TIME+ COMMAND                             
31824 root      20   0 2101196 2.001g    900 S   0.0 51.8   0:04.41 copyonwrite                         
31831 root      20   0 2101196 2.000g      0 S   0.0 51.8   0:00.03 copyonwrite                         
31833 root      20   0 2101196 2.000g      0 S   0.0 51.8   0:00.00 copyonwrite                         
31835 root      20   0 2101196 2.000g      0 S   0.0 51.8   0:00.00 copyonwrite                         
31836 root      20   0 2101196 2.000g      0 S   0.0 51.8   0:00.00 copyonwrite                         
31830 root      20   0 2101196 2.000g      0 S   0.0 51.8   0:00.08 copyonwrite                         
31832 root      20   0 2101196 2.000g      0 S   0.0 51.8   0:00.03 copyonwrite                         
31834 root      20   0 2101196 2.000g      0 S   0.0 51.8   0:00.00 copyonwrite
```
#### Explanation
* The physical memory is not actually allocated under COW mechanism. It'll be allocated under first write to the mem region
* COW helps saving memory while the child processes just forked and only read the memory from parent.
#### References
https://redis.io/topics/faq#background-saving-fails-with-a-fork-error-under-linux-even-if-i-have-a-lot-of-free-ram
### Transparent Hugepage (THP)
#### Simulation
```C
#define MAPSIZE 1024*1024*1024l     // 1G
long size = MAPSIZE * atol(argv[1]);
...
int *ptr = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
madvise(ptr, MAPSIZE, MADV_HUGEPAGE);
```
#### Runtime
```console
# cat /sys/kernel/mm/transparent_hugepage/enabled
always [madvise] never
# cat /proc/meminfo  | grep Anon
AnonPages:       7696872 kB
AnonHugePages:   4098048 kB
```
```console
# perf stat -e dTLB-load,dTLB-load-misses,cycles -- ./advise 8

 Performance counter stats for './advise 2000':

        34,435,997      dTLB-load                                                   
            26,798      dTLB-load-misses          #    0.08% of all dTLB cache hits 
     5,151,598,257      cycles                   

       2.135126528 seconds time elapsed

# perf stat -e dTLB-load,dTLB-load-misses,cycles -- ./no_advise 8

 Performance counter stats for './no_advise 2000':

     1,064,333,895      dTLB-load                                                   
         1,563,275      dTLB-load-misses          #    0.15% of all dTLB cache hits 
     8,797,338,489      cycles                   

       3.508520242 seconds time elapsed
```

#### Not a silver bullet
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
# time ./advise 20

real  0m22.884s
user  0m1.652s
sys	  0m21.244s

# time ./no_advise 20

real  0m8.520s
user  0m1.240s
sys   0m7.272s

# cat /proc/buddyinfo 
Node 0, zone   Normal  64522 171562 156286 108640  56285  17902   3005    322    121     59   1199 
Node 1, zone   Normal 253404 235857 186478  94067  28862   4424    463     66     45     39   2565 

Allocation without compaction: (2565 + 1199) * 4M + (39 + 59) * 2M ~ 15G
  ```

#### Rerferences
https://alexandrnikitin.github.io/blog/transparent-hugepages-measuring-the-performance-impact/
