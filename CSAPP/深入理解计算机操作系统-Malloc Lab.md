## Malloc Lab

我们不使用 过难的算法进行书写

仅用隐式空闲链表，构造一个动态的内存分配

用隐式链表的话能得个50分左右，采用显式空闲链表+LIFO或者显式空闲链表+地址排序能得个80分左右，采用Segregated free  List（分离空闲链表）得分较高，有100分左右。

我们搞定50分即可

### PRE

**README：**[http://csapp.cs.cmu.edu/3e/README-malloclab

**说明：**http://csapp.cs.cmu.edu/3e/malloclab.pdf

**代码：**[http://csapp.cs.cmu.edu/3e/malloclab-handout.tar]

**复习：**[http://www.cs.cmu.edu/afs/cs/acade

我们需要下载完善的traces

https://github.com/Ethan-Yan27/CSAPP-Labs/tree/master/yzf-malloclab-handout/traces

同时在config.h中修改相关的路径

![image-20220112183815721](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220112183815721.png)

本次实验比较难，本lab的目的是用C语言编写一个正确高效和快速的**动态存储分配器**，即**malloc**，**free**，**realloc**，和**calloc**函数。主要是在mm.c这个文件中编写以下几个函数：

```
bool mm_init(void);
void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);
void *calloc(size_t nmemb, size_t size);
bool mm_checkheap(int);

malloclab.pdf中introduction是这么写的
int mm_init(void);
void *mm_malloc(size_t size);
void mm_free(void *ptr);
void *mm_realloc(void *ptr, size_t size);
```

存储器实现的必要技术：

针对空闲块的组织方法有以下三种：
a.隐式空闲链表(implicit free list)
b.显式空闲链表(explicit free list)
c.分离空闲链表(segregated free list)

查找空闲块的三个方法：
a.首次适应(first fit)
b.最佳适配(best fit)
c.下一次适配(next fit)

#### 背景知识

使用动态内存申请器（比如malloc）是为了为那些在程序运行过程中才能确定大小的数据结构申请虚拟内存空间。
动态内存申请器管理的区域被称作**堆**，如下图。

![image-20220112153355707](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220112153355707.png)

- 动态内存申请器将**堆**这块区域当作一系列大小不同的块来管理，块或者式已经申请的，或者是空闲的。
- 动态内存申请器有两种：
  - **显式申请器**：应用负责申请和释放内存空间。如C语言中的malloc和free；
  - **隐式申请器**：应用申请内存空间，但是不释放内存空间。如Java中的new和垃圾回收机制（garbage collection）

#### 相关函数

malloc：`void* malloc(size_t size)`

- 如果`size == 0`，返回NULL；
- 成功：返回一个指针，该指针指向一块至少有`size`个字节的内存块。
  - 保证内存块的payload是16 bytes 对齐。

free：`void free(void* bp)`

- 释放指针`bp`指向的内存块；
- `bp`必须是由malloc、calloc或realloc返回的指针；
- 释放内存块之后需要和邻近的空闲块合并。

calloc：`void *calloc(size_t elements, size_t size)`

- 申请一段内存空间，并将这段内存空间初始化为0。

realloc：`void *realloc(void *ptr, size_t size)`

- 改变之前申请的一段内存块的大小。

calloc 和malloc的区别

C 库函数 **void \*calloc(size_t nitems, size_t size)** 分配所需的内存空间，并返回一个指向它的指针。**malloc** 和 **calloc** 之间的不同点是，malloc 不会设置内存为零，而 calloc 会设置分配的内存为零。

#### 内部碎片和外部碎片

- 内部碎片 （Internal Fragmentation)
  *Space within an allocated block that cannot be used for storing data, because it is required for some of the manager’s data structures (e.g., headers, footers, and free-list pointers).*

![image-20220112153702692](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220112153702692.png)

​          当Payload大小（绿色部分）小于Block大小的时候就会出现内部碎片，图中两边的灰色部分就是内部碎片。灰色部分在进程占有这块存储块时无法被利用，直到进程释放它，或进程结束时，系统才有可能利用这个存储块。

- 外部碎片 (External Fragmentation)
  *Unused space between allocated blocks or at the end of the heap.*
  `#define SIZ sizeof(size_t)`

![image-20220112153723177](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220112153723177.png)

- 外部碎片指的是还没有被分配出去（不属于任何进程），但**由于太小了无法分配给申请内存空间的新进程的内存空闲区域**，即处于任何两个已分配区域或页面之间的**没有被使用的空闲存储块**。这些存储块的总和可以满足当前申请的长度要求，但是由于它们的**地址不连续**或其他原因，使得系统无法满足当前申请。

#### 吞吐量和利用率

**吞吐量**和**利用率**是这个lab考察分配器效率的两个因素：

- 吞吐量

  throughput

  - *Number of completed requests per unit time* 单位时间内完成的请求数 (kilo-operations per second or KOPS)
  - 吞吐量衡量的是分配器的运行速度，换言之也就是在整个堆中查找空闲内存块的速度。查找空闲块的方法。

- 利用率

  utilization

  - *The peak ratio between the aggregate amount of memory used by the driver (i.e., allocated via malloc but not yet freed via free) and the size of the heap used by your allocator.*
  - 利用率衡量的是分配器的内外部碎片，在编写过程中一定要减小内外部碎片的出现。

需要注意的是，吞吐量和利用率是一对相冲突的目标。吞吐量最大化的同时不能够让利用率最大化，反之也是。所以在提升利用率的同时需要保证相对快的运行速度。

#### 查找空闲块

在查找空闲块的过程中，当有多个空闲块满足条件时，有几个不同的策略

   一、首次适应算法（First Fit）：该算法从空闲分区链首开始查找，直至找到一个能满足其大小要求的空闲分区为止。然后再按照作业的大小，从该分区中划出一块内存分配给请求者，余下的空闲分区仍留在空闲分区链    中。

     特点： 该算法倾向于使用内存中低地址部分的空闲区，在高地址部分的空闲区很少被利用，从而保留了高地址部分的大空闲区。显然为以后到达的大作业分配大的内存空间创造了条件。
    
     缺点：低地址部分不断被划分，留下许多难以利用、很小的空闲区，而每次查找又都从低地址部分开始，会增加查找的开销。
    
    
二、最佳适应算法（Best Fit）：该算法总是把既能满足要求，又是最小的空闲分区分配给作业。为了加速查找，该算法要求将所有的空闲区按其大小排序后，以递增顺序形成一个空白链。这样每次找到的第一个满足要求的空闲区，必然是最优的。孤立地看，该算法似乎是最优的，但事实上并不一定。因为每次分配后剩余的空间一定是最小的，在存储器中将留下许多难以利用的小空闲区。同时每次分配后必须重新排序，这也带来了一定的开销。

  特点：每次分配给文件的都是最合适该文件大小的分区。

   缺点：内存中留下许多难以利用的小的空闲区。

- First Fit：使用第一个找到的空闲块；//首次适用

```c
static block_t *first_fit(size_t asize) {
    
    block_t *block;
    for (block = heap_start; get_size(block) > 0; block = find_next(block)) {
    
        if (!(get_alloc(block)) 
        	&& (asize <= get_size(block))) {
    
            return block;
        }
    }
    return NULL; // no fit found
}
```

- Next Fit：在找到第一个符合请求的空闲块时，在接下来的几个空闲块中继续找看看有没有小一些但符合请求的空闲块（减小外部碎片）
- Best Fit：找到一个完完全全符合请求大小的空闲块。

```c
static block_t *best_fit(size_t asize) {
    
    block_t *block;
    for (block = heap_start; get_size(block) > 0; block = find_next(block)) {
    
        if (!(get_alloc(block)) 
        	&& (asize == get_size(block))) {
    
            return block;
        }
    }
    return NULL; // no fit found
}
```

### 第一版隐式空闲链表

![image-20220112155439377](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220112155439377.png)

![image-20220112155507399](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220112155507399.png)

每个block的结构如下图所示，block = header + payload + padding，header表示目前块的大小，因为块的地址是对齐的，所以大小的最后的四位一定是0（因为16的二进制表示是00…010000），所以最后的四位可以用来表示别的信息。这里最后一位就表示是否是个空闲块（0表示空闲块，1表示已分配）

![image-20220112155529512](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220112155529512.png)

之前提到，如果free一个block之后需要和这个空闲块前后的空闲块进行合并。如果用上图的结构的话，就会出现和前一个空闲块合并的时候不知道前面空闲块的大小的情况，所以可以在optional padding的地方存一个footer，内容和header一样都表示大小，如下图所示。

![image-20220112155536692](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220112155536692.png)

```
typedef struct block {
    
    /** @brief Header contains size + allocation flag */
    word_t header;
    char payload[0];
    /*
     * @ Declaring footer here will create another space other than the payload
     * but we want to reuse the byte in the payload as footers
     */
} block_t;

```

这样就可以在合并的时候知道前面的空闲块大小，可以用`find_prev_footer`来定位footer。

```
static word_t *find_prev_footer(block_t *block) {
    
    // Compute previous footer position as one word before the header
    return &(block->header) - 1;
}
```

#### 合并的四种情况

![image-20220112155634389](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220112155634389.png)

运用最低位指示块的例子：

![image-20220112155642481](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220112155642481.png)

堆的结构

![image-20220112155654502](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220112155654502.png)

#### 1.4 总结

最后，隐式空闲链表会出现很多的内部碎片，因为我们使用了header和footer，header和footer都是不能改变且不是用来存payload的。在之后会讲到如何优化这种情况（利用header里的剩下三位0）

#### 开始

我们首先需要在上面补充我们的宏(书上有写)

![image-20220112172857612](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220112172857612.png)

![image-20220112172909334](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220112172909334.png)

![image-20220112172918737](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220112172918737.png)

```
/*Basic constants and macros */
#define WSIZE 4 /* Word and header/footer size (bytes) */
#define DSIZE 8 /* Double word size (bytes) */
#define CHUNKSIZE (1<<12) /* Extend heap by this amount (bytes) */

#define MAX(x, y) ((x) > (y)? (x) : (y))

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc) ((size) | (alloc))

/* Read and write a word at address p */
#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp) ((char *)(bp) - WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))
```





我们使用课本上的代码

![image-20220112171421940](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220112171421940.png)

```
void *mm_malloc(size_t size)
{
    site_t asize;
    size_t extendsize;
    char *bp;

    if(size = 0)
        return NULL;

    if(size <= DSIZE)
        asize = 2*DSIZE;
    else
        asize = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE);

    if(bp = find_fit(asize) != NULL){
        place(bp,asize);
        return bp;
    }

    extendsize = MAX(asize,CHUNKSIZE);
    if(( bp = extend_heap(extendsize/WSIZE)) == NULL)
        return NULL;
    place(bp,asize);
    return bp;
}
```

编写（抄写）完成后，make编译，遇到了问题

![image-20220112175445262](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220112175445262.png)

因为是64位系统，使用了-m32

安装这个

```
sudo apt install libc6-dev-i386
```

隐式空闲链表+首次适配+原始realloc版

最终代码

```
/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 *
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "wum1ng",
    /* First member's full name */
    "Harry Bovik",
    /* First member's email address */
    "bovik@cs.cmu.edu",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/*Basic constants and macros */
#define WSIZE 4 /* Word and header/footer size (bytes) */
#define DSIZE 8 /* Double word size (bytes) */
#define CHUNKSIZE (1<<12) /* Extend heap by this amount (bytes) */

#define MAX(x, y) ((x) > (y)? (x) : (y))

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc) ((size) | (alloc))

/* Read and write a word at address p */
#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp) ((char *)(bp) - WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

static void *extend_heap(size_t words);
static void *find_fit(size_t asize);
static void place(void *bp, size_t asize);
static void *coalesce(void *bp);

static char *heap_listp;
static char *pre_listp;

static void *extend_heap(size_t words)
{
    char *bp;
    size_t size;

    /* Allocate an even number of words to maintain alignment */
    size = (words % 2) ? (words+1) * WSIZE : words * WSIZE;
    if ((long)(bp = mem_sbrk(size)) == -1)
        return NULL;

    /* Initialize free block header/footer and the epilogue header */

    PUT(HDRP(bp), PACK(size, 0)); /* Free block header */
    PUT(FTRP(bp), PACK(size, 0)); /* Free block footer */
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); /* New epilogue header */

    /* Coalesce if the previous block was free */

    return coalesce(bp);
}

/*
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    /* Create the initial empty heap */
    if ((heap_listp = mem_sbrk(4*WSIZE)) == (void *)-1)
        return -1;
    PUT(heap_listp, 0); /* Alignment padding */
    PUT(heap_listp + (1*WSIZE), PACK(DSIZE, 1)); /* Prologue header */
    PUT(heap_listp + (2*WSIZE), PACK(DSIZE, 1)); /* Prologue footer */
    PUT(heap_listp + (3*WSIZE), PACK(0, 1)); /* Epilogue header */
    heap_listp += (2*WSIZE);
    pre_listp = heap_listp;
    /* Extend the empty heap with a free block of CHUNKSIZE bytes */
    if (extend_heap(CHUNKSIZE/WSIZE) == NULL)
        return -1;
    return 0;
}

static void *find_fit(size_t asize)
{
    char *bp = heap_listp;
    size_t alloc;
    size_t size;
    while (GET_SIZE(HDRP(NEXT_BLKP(bp))) > 0) {
        bp = NEXT_BLKP(bp);
        alloc = GET_ALLOC(HDRP(bp));
        if (alloc) continue;
        size = GET_SIZE(HDRP(bp));
        if (size < asize) continue;
        return bp;
    }
    return NULL;
}


static void place(void *bp, size_t asize)
{
    size_t size = GET_SIZE(HDRP(bp));

    if ((size - asize) >= (2*DSIZE)) {
        PUT(HDRP(bp),PACK(asize,1));
        PUT(FTRP(bp),PACK(asize,1));
        PUT(HDRP(NEXT_BLKP(bp)),PACK(size - asize,0));
        PUT(FTRP(NEXT_BLKP(bp)),PACK(size - asize,0));
    } else {
        PUT(HDRP(bp),PACK(size,1));
        PUT(FTRP(bp),PACK(size,1));
    }
}

/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    size_t asize; /* Adjusted block size */
    size_t extendsize; /* Amount to extend heap if no fit */
    char *bp;

    /* Ignore spurious requests */
    if (size == 0)
        return NULL;

    /* Adjust block size to include overhead and alignment reqs. */
    if (size <= DSIZE)
        asize = 2*DSIZE;
    else
        asize = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE);

    /* Search the free list for a fit */
    if ((bp = find_fit(asize)) != NULL) {
        place(bp, asize);
        return bp;
    }

    /* No fit found. Get more memory and place the block */
    extendsize = MAX(asize,CHUNKSIZE);
    if ((bp = extend_heap(extendsize/WSIZE)) == NULL)
        return NULL;
    place(bp, asize);
    return bp;
}

static void *coalesce(void *bp){
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if(prev_alloc && next_alloc){/*case 1*/
        return bp;
    }
    else if(prev_alloc && !next_alloc){/*case 2*/
        size +=GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp),PACK(size,0));
        PUT(FTRP(bp),PACK(size,0));
    }
    else if(!prev_alloc && next_alloc){/*case 3 */
        size +=GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp),PACK(size,0));
        PUT(HDRP(PREV_BLKP(bp)),PACK(size,0));
        bp = PREV_BLKP(bp);
    }
    else{/*case 4*/
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)),PACK(size,0));
        PUT(FTRP(PREV_BLKP(bp)),PACK(size,0));
        bp = PREV_BLKP(bp);
    }
    return bp;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *bp)
{
    size_t size = GET_SIZE(HDRP(bp));

    PUT(HDRP(bp),PACK(size,0));
    PUT(FTRP(bp),PACK(size,0));
    coalesce(bp);
}


/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    if (ptr == NULL)
       return mm_malloc(size);
    if (size == 0) {
       mm_free(ptr);
       return NULL;
    }

    void *newptr;
    size_t copySize;

    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;
    size = GET_SIZE(HDRP(ptr));
    copySize = GET_SIZE(HDRP(newptr));
    if (size < copySize)
      copySize = size;
    memcpy(newptr, ptr, copySize - WSIZE);
    mm_free(ptr);
    return newptr;
}














```

![image-20220112194327009](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220112194327009.png)

这个代码只能跑最基本的代码，跑全的话，就会卡在reading tracefile

我复制了github上的代码，非常快的出结果

发现是代码的问题，已经修改了

```
./mdriver -V
Team Name:wum1ng
Member 1 :Harry Bovik:bovik@cs.cmu.edu
Using default tracefiles in /root/csapp/malloclab-handout/traces/
Measuring performance with gettimeofday().

Testing mm malloc
Reading tracefile: amptjp-bal.rep
Checking mm_malloc for correctness, efficiency, and performance.
Reading tracefile: cccp-bal.rep
Checking mm_malloc for correctness, efficiency, and performance.
Reading tracefile: cp-decl-bal.rep
Checking mm_malloc for correctness, efficiency, and performance.
Reading tracefile: expr-bal.rep
Checking mm_malloc for correctness, efficiency, and performance.
Reading tracefile: coalescing-bal.rep
Checking mm_malloc for correctness, efficiency, and performance.
Reading tracefile: random-bal.rep
Checking mm_malloc for correctness, efficiency, and performance.
Reading tracefile: random2-bal.rep
Checking mm_malloc for correctness, efficiency, and performance.
Reading tracefile: binary-bal.rep
Checking mm_malloc for correctness, efficiency, and performance.
Reading tracefile: binary2-bal.rep
Checking mm_malloc for correctness, efficiency, and performance.
Reading tracefile: realloc-bal.rep
Checking mm_malloc for correctness, efficiency, and performance.
Reading tracefile: realloc2-bal.rep
Checking mm_malloc for correctness, efficiency, and performance.

Results for mm malloc:
trace  valid  util     ops      secs  Kops
 0       yes   99%    5694  0.007775   732
 1       yes   99%    5848  0.006857   853
 2       yes   99%    6648  0.011407   583
 3       yes  100%    5380  0.008158   660
 4       yes   66%   14400  0.000096150785
 5       yes   92%    4800  0.006561   732
 6       yes   92%    4800  0.006738   712
 7       yes   55%   12000  0.142871    84
 8       yes   51%   24000  0.308106    78
 9       yes   27%   14401  0.075602   190
10       yes   34%   14401  0.003239  4446
Total          74%  112372  0.577409   195

Perf index = 44 (util) + 13 (thru) = 57/100



```

最终得分57分

跑出来的峰值利用率还挺高，但是吞吐量很小，因为隐式空闲链表每次malloc的时候需要从头遍历所有的块，以及立即合并可能会产生抖动。

下面来分析代码

#### 分析代码

宏定义

![image-20220112212731910](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220112212731910.png)

![image-20220112212740788](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220112212740788.png)

````
/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
// 向上舍入到8字节对齐
// 对~0x7求与后将低三位变成了0，也就是与8字节对齐
// 加上7保证了向上舍入
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/*Basic constants and macros */
// 字的大小
#define WSIZE 4 /* Word and header/footer size (bytes) */
// 双字的大小
#define DSIZE 8 /* Double word size (bytes) */
// 扩展堆时的默认大小，4KB，1左移12位，想你大概与2的12次方
#define CHUNKSIZE (1<<12) /* Extend heap by this amount (bytes) */

//求最大值
#define MAX(x, y) ((x) > (y)? (x) : (y))

// 将一个块的大小（size）和分配标志位（allocated bit）打包成一个字
// 分配标志位占据低三位
/* Pack a size and allocated bit into a word */
#define PACK(size, alloc) ((size) | (alloc))

// 在地址p处，读取和写入一个字
// 参数p典型地是一个（void *）指针，不可以直接进行解引用，
// 要读取一个字,需要先将泛型指针转化为32位类型的指针（比如unsigned int），
// 再进行解引用
/* Read and write a word at address p */
#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))

// 从地址p的头部或脚部分别返回大小和已分配标志位
/* Read the size and allocated fields from address p */
#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

// 给定一个块指针bp，分别返回指向这个块的头部和脚部的指针
/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp) ((char *)(bp) - WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

// 给定一个块指针bp，分别返回指向后面块和前面的块的块指针
/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

````

mm_init

堆初始化



![image-20220112212559094](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220112212559094.png)

```
/*
 * mm_init - initialize the malloc package.
 */
// 在调用mm_malloc或者mm_free之前，应用必须先调用mm_init函数来初始化堆
int mm_init(void)
{
    // 从内存系统得到四个字
    // 从void*隐式转化为char* 在C中可以，在C++中不行，需要使用static_cast进行强制类型转换
    /* Create the initial empty heap */
    if ((heap_listp = mem_sbrk(4*WSIZE)) == (void *)-1)
        return -1;
    // 初始化堆
    // 初始化填充块
    PUT(heap_listp, 0); /* Alignment padding */
    // 初始化序言块。
    // 序言块和结尾块总是标记为已分配，用来消除合并时的边界条件
    // 否则我们在释放块时，需要先判断bp前后是否有块，在来判断是否空闲。
    PUT(heap_listp + (1*WSIZE), PACK(DSIZE, 1)); /* Prologue header */
    PUT(heap_listp + (2*WSIZE), PACK(DSIZE, 1)); /* Prologue footer */
    // 初始化结尾块
    PUT(heap_listp + (3*WSIZE), PACK(0, 1)); /* Epilogue header */
    heap_listp += (2*WSIZE);
    pre_listp = heap_listp;

    // 将这个堆扩展 CHUNKSIZE个字节，并创建初始的空闲块
    /* Extend the empty heap with a free block of CHUNKSIZE bytes */
    if (extend_heap(CHUNKSIZE/WSIZE) == NULL)
        return -1;
    return 0;
}
```

```
static void *extend_heap(size_t words)
{
    char *bp;
    size_t size;

    // 向上舍入，分配偶数个字来保持双字对齐
    /* Allocate an even number of words to maintain alignment */

    size = (words % 2) ? (words+1) * WSIZE : words * WSIZE;
    if ((long)(bp = mem_sbrk(size)) == -1)
        return NULL;
    // mem_sbrk给堆新分配的内存片的地址在之前的结尾块的头部后面

    // 这个头部就变成了新的空闲块的头部，并且这个片的最后一个字变成了
    // 新的结尾块的头部,倒数第二个字变成了新的空闲块的尾部
    /* Initialize free block header/footer and the epilogue header */
    PUT(HDRP(bp), PACK(size, 0)); /* Free block header */
    PUT(FTRP(bp), PACK(size, 0)); /* Free block footer */
    // 结尾块的头部
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); /* New epilogue header */

    /* Coalesce if the previous block was free */
    // 如果前一个块是空闲块，就合并，并返回指向合并后的块的块指针
    return coalesce(bp);
}
```



![image-20220112212818823](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220112212818823.png)

![image-20220112212828781](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220112212828781.png)

mm_free

```
// 合并空闲块，使用场景：一、释放块时；二、内存系统给堆分配新的块时
static void *coalesce(void *bp){
    // 获取前后两个相邻块的分配标志位
    void *prev_bp = PREV_BLKP(bp);
    void *next_bp = NEXT_BLKP(bp);
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));

    size_t size = GET_SIZE(HDRP(bp));
    // 判断相邻的块是否为空闲块，如果是空闲块，就要将它们从空闲链表中移除
    // 然后与bp指向的块合并，
    // 如果前后的两个块已分配
    if(prev_alloc && next_alloc){/*case 1*/
       // pre_listp = (char*)bp;
        return bp;
    }
    // 如果前面的块已分配，后面的块未分配
    else if(prev_alloc && !next_alloc){/*case 2*/
        // 与bp指向的块合并
        size +=GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp),PACK(size,0));
        PUT(FTRP(bp),PACK(size,0));
    }
    else if(!prev_alloc && next_alloc){/*case 3 */
        size +=GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp),PACK(size,0));
        PUT(HDRP(PREV_BLKP(bp)),PACK(size,0));
        bp = PREV_BLKP(bp);
    }
    else{/*case 4*/
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)),PACK(size,0));
        PUT(FTRP(PREV_BLKP(bp)),PACK(size,0));
        bp = PREV_BLKP(bp);
    }
    return bp;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *bp)
{
     // 将要释放的块的头部和脚部的分配标志位改为0即可
    size_t size = GET_SIZE(HDRP(bp));

    PUT(HDRP(bp),PACK(size,0));
    PUT(FTRP(bp),PACK(size,0));

    // 释放时需要与相邻的空闲块合并
    coalesce(bp);
}

```

![image-20220112213004823](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220112213004823.png)

mm_malloc

![image-20220112213114261](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220112213114261.png)

```
/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    // asize和 extendsize的单位都是字节
    // 分配的块的大小
    size_t asize; /* Adjusted block size */
    // 如果没有匹配，堆应该扩展的大小
    size_t extendsize; /* Amount to extend heap if no fit */
    char *bp;

    /* Ignore spurious requests */
    if (size == 0)
        return NULL;

     // 8字节用来放头部和脚部，而又不分配有效载荷为0的块，所以对齐后最小块的大小为16字节
    /* Adjust block size to include overhead and alignment reqs. */
    if (size <= DSIZE)
        asize = 2*DSIZE;
    else
        // 块的大小为：应用请求分配的有效载荷 加上 开销字节（头部和脚部）然后再向上舍入到最接近的8字节的整数倍
        asize = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE);

    // 分配器确定的了请求的块的大小后，搜索空闲链表来寻找一个合适的空闲块
    /* Search the free list for a fit */
    if ((bp = find_fit(asize)) != NULL) {
        // 如果有合适的，那么就在bp指向的位置放置asize大小的请求块，并可选地分割出多余的部分
        place(bp, asize);
        return bp;
    }

    // 如果没有合适的空闲块，就向系统请求更多的空间，用一个新的空闲块来扩展堆，
    // 再把请求块放置在这个新的空闲块中，然后返回一个指针，指向这个新分配的块
    /* No fit found. Get more memory and place the block */
    extendsize = MAX(asize,CHUNKSIZE);
    if ((bp = extend_heap(extendsize/WSIZE)) == NULL)
        return NULL;
    place(bp, asize);
     // 然后返回新分配块的地址
    return bp;
}
```

![image-20220112213141134](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220112213141134.png)

find_fit

采用的首次适配算法

```
static void *find_fit(size_t asize)
{
    char *bp = heap_listp;
    size_t alloc;
    size_t size;
    while (GET_SIZE(HDRP(NEXT_BLKP(bp))) > 0) {
        bp = NEXT_BLKP(bp);
        alloc = GET_ALLOC(HDRP(bp));
        if (alloc) continue;
        size = GET_SIZE(HDRP(bp));
        if (size < asize) continue;
        return bp;
    }
    return NULL;
}
```



place

```
// 在bp指向的空闲块处放置请求块，当剩余的部分大于等于最小块时，才将此空闲块分割
// 将bp指向的块从空闲链表中移除，将分割出来的空闲块与相邻的空闲块合并，合并完后插入空闲链表的头部
static void place(void *bp, size_t asize)
{
    size_t size = GET_SIZE(HDRP(bp));

    if ((size - asize) >= (2*DSIZE)) {
        //bp块的头部中的块大小变成了asize
        PUT(HDRP(bp),PACK(asize,1));
        PUT(FTRP(bp),PACK(asize,1));
        PUT(HDRP(NEXT_BLKP(bp)),PACK(size - asize,0));
        PUT(FTRP(NEXT_BLKP(bp)),PACK(size - asize,0));
    } else {
        // 将空闲块的分配标志位改为1
        PUT(HDRP(bp),PACK(size,1));
        PUT(FTRP(bp),PACK(size,1));
    }
}
```



#### 改进

可以选用显式空闲链表，最佳适配，改进realloc

![image-20220112214317124](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220112214317124.png)

当然现在最优的是

Segregated Free Lists

![image-20220112214431571](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220112214431571.png)

去除非空闲块的footer

#### 去除非空闲块的footer

第一个我们可以进行优化的是去掉**非空闲块**的footer，这个方法在之前1.4中提到过，可以用来减少内部碎片。
之前我们需要footer的原因是方便我们在free完成之后合并前后的空闲块，但现在我们可以把前面是不是空闲块这个信息也存储在表示大小的header里面最后四位的指示块中。为了防止有错误出现，这里建议先从有footer的情况开始，如下图所示，只是在header处把倒数第二位变成判断之前一个块是不是空闲块的信息（回想：最后一位表示自己这个块是不是空闲的），注意这里我们只去掉**非空闲块**的footer，空闲块的footer还是得以保留。

![image-20220112214644111](https://gitee.com/dingpengs/image/raw/master/imgwin/image-20220112214644111.png)