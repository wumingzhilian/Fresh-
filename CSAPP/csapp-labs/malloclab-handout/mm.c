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

static void *extend_heap(size_t words);
static void *find_fit(size_t asize);
static void place(void *bp, size_t asize);
static void *coalesce(void *bp);

static char *heap_listp;
// 上一次在堆中匹配的位置，用于next_fit，从此处开始匹配
static char *pre_listp;

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













