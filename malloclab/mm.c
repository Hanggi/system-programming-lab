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
 * provide your information in the following struct.
 ********************************************************/
team_t team = {
  /* Team name : Your student ID */
  "2016-27542",
  /* Your full name */
  "HANGQI CUI",
  /* Your student ID */
  "2016-27542",
  /* leave blank */
  "",
  /* leave blank */
  ""
};

/* DON'T MODIFY THIS VALUE AND LEAVE IT AS IT WAS */
static range_t **gl_ranges;

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/* ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ *
 * defintion of Macros & Header Comment
 * ■■■■                                                                 ■■■■■■ */

/*	Header Comment

	Structure:
	My structure of free and allocated blocks referenced the structure show in the pdf and the textbook.
	The memory block start with 8bit aligned header and end up with 8bit aligned footer.and the content also aligned with 8bit.

	Free list:
	In mm_init() I expanded the heap by 14 words for the free list and other initialization, because we can not use any array or struct. 
	The first 10 words is for free, and I used best fit to get a better performance.
	The first word for allocation request less than 32, the seccond less than 64, and so on. The last one is bigger 16384, because I have seen the *.rep files, there hardly have a size bigger than 16384.
	And last 4 words are for Prologue and Epilogue.

	Manipulate:
	Every time we free a block, coalesce the near free block or split the big free block, I will insert a free block infomation to the proper size of free list.
	And every time we alloc a block, before coalesce a near free block or split the big free block, I will remove the old free block from the free list.


*/

#define WSIZE 4                     /*word size*/
#define DSIZE 8                     /*Double word size*/
#define CHUNKSIZE (1<<12)           /*the page size in bytes is 4K*/
#define MINFB_SIZE 2

#define MAX(x,y)    ((x)>(y)?(x):(y))

/* Pack a size and allocated bit into a word */
#define PACK(size,alloc)    ((size) | (alloc))

/* Read the size and allocated fields from address p */
#define GET(p)  (*(unsigned int *)(p))              /*get value of 4 sizes memory */
#define PUT(p,val)  (*(unsigned int *)(p) = (val))  /*put val into 4 sizes memory */


/* Read the size and allocated fields from address p */
#define GET_SIZE(p)  (GET(p) & ~0x7)      /*size of block*/
#define GET_ALLOC(p)    (GET(p) & 0x1)    /*block is alloc?*/

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp)    ((char *)(bp)-WSIZE)                    /*head of block*/
#define FTRP(bp)    ((char *)(bp)+GET_SIZE(HDRP(bp))-DSIZE) /*foot of block*/

/*Given free block ptr bp ,compute addredss of next ane previous free block */
#define PREV_LINKNODE_RP(bp) ((char*)(bp))          /*prev free block*/
#define NEXT_LINKNODE_RP(bp) ((char*)(bp)+WSIZE)    /*next free block*/
/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp)   ((char *)(bp)+GET_SIZE(((char *)(bp)-WSIZE))) /*next block*/
#define PREV_BLKP(bp)   ((char *)(bp)-GET_SIZE(((char *)(bp)-DSIZE))) /*prev block*/


static void *extend_heap(size_t dwords);
static void *coalesce(void *bp);
static void *find_fit(size_t size);
static void place(void *bp,size_t asize);
/*inline function*/
void insert_free_block(char *p);
void remove_free_block(char *p);
char *get_freeCategory_root(size_t size);

// static void *realloc_coalesce(void *bp,size_t newSize,int *isNextFree);
// static void realloc_place(void *bp,size_t asize);

static char *heap_listp = NULL;
static char *block_list_start = NULL;

/* Definition of the DEBUG and MM_CHECKER */
// #define DEBUG
// #define MMCHECK

/* ■■■■                                                                 ■■■■■■ */
/* ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ */


/* 
 * remove_range - manipulate range lists
 * DON'T MODIFY THIS FUNCTION AND LEAVE IT AS IT WAS
 */
static void remove_range(range_t **ranges, char *lo)
{
  range_t *p;
  range_t **prevpp = ranges;
  
  if (!ranges)
    return;

  for (p = *ranges;  p != NULL; p = p->next) {
    if (p->lo == lo) {
      *prevpp = p->next;
      free(p);
      break;
    }
    prevpp = &(p->next);
  }
}

/*
 * mm_init - initialize the malloc package.
 */
int mm_init(range_t **ranges)
{
  /* YOUR IMPLEMENTATION */

/* ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ *
 * Code of mm_init()
 * ■■■■                                                                  ■■■■■ */
	if((heap_listp = mem_sbrk(14*WSIZE))==(void *)-1) return -1;

		PUT(heap_listp,0);              				/*block size list<=32*/
		PUT(heap_listp+(1*WSIZE),0);   				 	/*block size list<=64*/
		PUT(heap_listp+(2*WSIZE),0);    				/*block size list<=128*/
		PUT(heap_listp+(3*WSIZE),0);    				/*block size list<=256*/
		PUT(heap_listp+(4*WSIZE),0);    				/*block size list<=512*/
		PUT(heap_listp+(5*WSIZE),0);   				 	/*block size list<=2048*/
		PUT(heap_listp+(6*WSIZE),0);    				/*block size list<=4096*/
		PUT(heap_listp+(7*WSIZE),0);    				/*block size list<=8192*/
		PUT(heap_listp+(8*WSIZE),0);    				/*block size list<=16384*/
		PUT(heap_listp+(9*WSIZE),0);    				/*block size list>16384*/
		PUT(heap_listp+(10*WSIZE),0);					/* for alignment*/
		PUT(heap_listp+(11*WSIZE),PACK(DSIZE,1));   	/* Prologue header */
		PUT(heap_listp+(12*WSIZE),PACK(DSIZE,1));		/* Prologue footer */
		PUT(heap_listp+(13*WSIZE),PACK(0,1));			/* Epilogue header */

		block_list_start = heap_listp;
		heap_listp += (12*WSIZE);

		#ifdef DEBUG
			printf("\n\n\033[1;33m■ ■■■■■■■■■■■■■■■■■■■ ■\033[0m\n■ \033[1;33m初始化heap_star: %p\033[0m \n■   \033[1;35mstart address: %p \033[0m", heap_listp, block_list_start);
		#endif

		if((extend_heap(CHUNKSIZE/DSIZE))==NULL) return -1;
/* ■■■■                                                                 ■■■■■■ */
/* ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ */

  /* DON't MODIFY THIS STAGE AND LEAVE IT AS IT WAS */
  gl_ranges = ranges;

  return 0;
}

/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void* mm_malloc(size_t size)
{
  // int newsize = ALIGN(size + SIZE_T_SIZE);
  // void *p = mem_sbrk(newsize);
  // if (p == (void *)-1)
  //   return NULL;
  // else {
  //   *(size_t *)p = size;
  //   return (void *)((char *)p + SIZE_T_SIZE);
  // }
/* ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ *
 * Code of mm_malloc
 * ■■■■                                                                 ■■■■■■ */
    size_t asize;
    size_t extendsize;
    char *bp;
    if(size ==0) return NULL;

    if(size <= DSIZE){
        asize = 2*(DSIZE);
    }else{
        asize = (DSIZE)*((size+(DSIZE)+(DSIZE-1)) / (DSIZE));
    }

	#ifdef DEBUG
		printf("\n\033[1;33m■\033[0m\ \033[1;34m进入Malloc\033[0m \n");
		printf("■        \033[1;36m申请size: %d\033[0m \n", size);
		printf("■      \033[0;36m处理后size: %d \033[0m", asize);
	#endif
    
    if((bp = find_fit(asize))!= NULL){
        place(bp,asize);

		#ifdef DEBUG
			printf("\n■       \033[1;36m头address: %p\033[0m", bp);
			printf("\n■      \033[1;36m get_Alloc: %d\033[0m", GET_ALLOC(HDRP(bp)));
		#endif

        return bp;
    }

    /*apply new block*/
    extendsize = MAX(asize,CHUNKSIZE);
    if((bp = extend_heap(extendsize/DSIZE))==NULL){
        return NULL;
    }
    place(bp,asize);

	#ifdef DEBUG
		printf("\n■       \033[1;33m经过一次extend\033[0m");
		printf("\n■       \033[1;36m头address: %p\033[0m", bp);
		printf("\n■      \033[1;36m get_Alloc: %d\033[0m", GET_ALLOC(HDRP(bp)));
	#endif

    return bp;
/* ■■■■                                                                 ■■■■■■ */
/* ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ */
}

/*
 * mm_free - Freeing a block.
 */
void mm_free(void *bp)
{
  /* YOUR IMPLEMENTATION */
/* ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ *
 * Code of mm_free
 * ■■■■                                                                 ■■■■■■ */
   if(bp == 0)
        return;
    size_t size = GET_SIZE(HDRP(bp));

	#ifdef DEBUG
		printf("\n\033[0;32;32m■ 进入Free\033[0m \n");
		printf("■          \033[0;32;31m释放address: %p\033[0m \n", bp);
		printf("■             \033[1;31m释放大小: %d \033[0m\n", size);
	#endif

    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    PUT(NEXT_LINKNODE_RP(bp),NULL);
    PUT(PREV_LINKNODE_RP(bp),NULL);
    coalesce(bp);

/* ■■■■                                                                 ■■■■■■ */
/* ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ */

  /* DON't MODIFY THIS STAGE AND LEAVE IT AS IT WAS */
  if (gl_ranges)
    remove_range(gl_ranges, bp);
}

/*
 * mm_realloc - empty implementation; YOU DO NOT NEED TO IMPLEMENT THIS
 */
void* mm_realloc(void *ptr, size_t t)
{
  return NULL;
}

/*
 * mm_exit - finalize the malloc package.
 */
void mm_exit(void)
{
    #ifdef MMCHECK
        mm_check();
    #endif
/* ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ *
 * Code of mm_exit()
 * ■■■■                                                                 ■■■■■■ */
    void* p2 = mem_heap_hi();
	#ifdef DEBUG
    void* p1 = mem_heap_lo();
    int i1 = mem_heapsize();
    int i2 = mem_pagesize();

	// printf("\n%p \n", p1);
	// printf("%p \n", p2);
	// printf("%d \n", i1);
	// printf("%d \n", i2);
	#endif

	void* p = heap_listp + 8;

	while (p) {

		// size_t  foot_alloc = GET_ALLOC(FTRP(p));
		size_t  head_alloc = GET_ALLOC(HDRP(p));

		if (head_alloc) {
			#ifdef DEBUG
			printf("\n\033[0;35mMemory in %p is %d unfreed\033[0m\n", p, head_alloc);
			#endif

			mm_free(p);
		}else {
			#ifdef DEBUG
			printf("\n\033[0;32;32mMemory in %p is %d freed\033[0m\n", p, head_alloc);
			#endif
		}
		p = NEXT_BLKP(p);

		if (p > p2) {
			#ifdef DEBUG
			printf("%p > %p \n", p, p1);
			#endif
			p = NULL;
		}

	}

    

/* ■■■■                                                                 ■■■■■■ */
/* ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ */
}


/* ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ *
 * Implementation of functions
 * ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ */
/*
 *	get_freeCategory_root - get the free list category fit the given size
 */
inline char *get_freeCategory_root(size_t size)
{
    int i = 0;
    if(size<=8) i=0;
    else if(size<=16) i= 0;
    else if(size<=32) i= 0;
    else if(size<=64) i= 1;
    else if(size<=128) i= 2;
    else if(size<=256) i= 3;
    else if(size<=512) i= 4;
    else if(size<=2048) i= 5;
    else if(size<=4096) i= 6;
    else if(size<=8192) i= 7;
    else if(size<=16384) i= 8;
    else i= 9;
    /*find the index of bin which will put this block */
    return block_list_start+(i*WSIZE);
}
/*
 *extend_heap - Extend heap with free block and return its block pointer.
 */
static void *extend_heap(size_t dwords)
{
    char *bp;
    size_t size;

    size = (dwords % 2) ? (dwords+1) * DSIZE : dwords * DSIZE; /*compute the size fit the 16 bytes alignment*/

    if((long)(bp = mem_sbrk(size))==-1)		/*move the brk pointer for bigger heap*/
        return NULL;

    /*init the head and foot fields*/
    PUT(HDRP(bp),PACK(size,0));
    PUT(FTRP(bp),PACK(size,0));

    /*init the prev and next free pointer fields*/
    PUT(NEXT_LINKNODE_RP(bp),NULL);
    PUT(PREV_LINKNODE_RP(bp),NULL);

    /*the  epilogue header*/
    PUT(HDRP(NEXT_BLKP(bp)),PACK(0,1));

    return coalesce(bp);
}
/*
*coalesce - Boundary tag coalescing. Return ptr to coalesced block
*/
static void *coalesce(void *bp)
{
    size_t  prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t  next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));


    /*coalesce the block and change the point*/
    if(prev_alloc && next_alloc){		//case1
    }
    else if(prev_alloc && !next_alloc)  //case2
    {
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        remove_free_block(NEXT_BLKP(bp)); /*remove the next free block from the free list */
        PUT(HDRP(bp), PACK(size,0));
        PUT(FTRP(bp), PACK(size,0));
    }
    else if(!prev_alloc && next_alloc)	//case 3
    {
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        remove_free_block(PREV_BLKP(bp)); /*remove the prev free block from the free list */
        PUT(FTRP(bp),PACK(size,0));
        PUT(HDRP(PREV_BLKP(bp)),PACK(size,0));
        bp = PREV_BLKP(bp);
    }
    else	//case 4
    {
        size +=GET_SIZE(FTRP(NEXT_BLKP(bp)))+ GET_SIZE(HDRP(PREV_BLKP(bp)));
        remove_free_block(PREV_BLKP(bp));
        remove_free_block(NEXT_BLKP(bp));
        PUT(FTRP(NEXT_BLKP(bp)),PACK(size,0));
        PUT(HDRP(PREV_BLKP(bp)),PACK(size,0));
        bp = PREV_BLKP(bp);
    }
    /*insert the new free block*/
    insert_free_block(bp);

    return bp;
}
/*
*insert_free_block - insert the free point into segragated free list.
*	In each category the free list is ordered by the free size for small to big.
*   When find a fit free block ,just find for begin to end ,the first fit one is the best fit one.
*/
inline void insert_free_block(char *p)
{
    char *root = get_freeCategory_root(GET_SIZE(HDRP(p)));
    char *prevp = root;
    char *nextp = GET(root);

    /*find the postion to insert, smaller < p < bigger*/
    while(nextp!=NULL){
        if(GET_SIZE(HDRP(nextp))>=GET_SIZE(HDRP(p))) break;
        prevp = nextp;
        nextp = GET(NEXT_LINKNODE_RP(nextp));
    }
    /*insert*/
    if(prevp == root){
        PUT(root,p);
        PUT(NEXT_LINKNODE_RP(p),nextp);
        PUT(PREV_LINKNODE_RP(p),NULL);
        if(nextp!=NULL) PUT(PREV_LINKNODE_RP(nextp),p);
    }else{
        PUT(NEXT_LINKNODE_RP(prevp),p);
        PUT(PREV_LINKNODE_RP(p),prevp);
        PUT(NEXT_LINKNODE_RP(p),nextp);
        if(nextp!=NULL) PUT(PREV_LINKNODE_RP(nextp),p);
    }

}
/*
*remove_free_block - remove the free point from segragated free list.
*/
inline void remove_free_block(char *p)
{
    char *root = get_freeCategory_root(GET_SIZE(HDRP(p)));
    char *prevp = GET(PREV_LINKNODE_RP(p));
    char *nextp = GET(NEXT_LINKNODE_RP(p));

    if(prevp == NULL){
        if(nextp != NULL)PUT(PREV_LINKNODE_RP(nextp),0);
        PUT(root,nextp);
    } else {
        if(nextp != NULL)PUT(PREV_LINKNODE_RP(nextp),prevp);
        PUT(NEXT_LINKNODE_RP(prevp),nextp);
    }
    /*set the next and prev pointers to NULL*/
    PUT(NEXT_LINKNODE_RP(p),NULL);
    PUT(PREV_LINKNODE_RP(p),NULL);
}
/* 
 * find_fit - Find a fit for a block with asize bytes .(best fit alogrithm)
 *       	  Because the free list is order by size, so find the first fit one is the same as the best fit one.
 */
static void *find_fit(size_t size)
{
    
    char *root = get_freeCategory_root(size);
    char *tmpP = GET(root);
    for(root; root!=(heap_listp-(2*WSIZE)); root+=WSIZE)
    {
        char *tmpP = GET(root);
        while(tmpP != NULL)
        {
            if(GET_SIZE(HDRP(tmpP))>=size) return tmpP;
            tmpP = GET(NEXT_LINKNODE_RP(tmpP));
        }
    }
    return NULL;
}
/* 
 * place - Place block of asize bytes at start of free block bp 
 *         and split if remainder would be at least minimum block size
 */
static void place(void *bp,size_t asize)
{
    size_t csize = GET_SIZE(HDRP(bp));
    remove_free_block(bp);/*remove from empty_list*/
    if((csize-asize)>=(MINFB_SIZE*DSIZE))
    {
		// if the free block bigger than asize
        PUT(HDRP(bp),PACK(asize,1));
        PUT(FTRP(bp),PACK(asize,1));
        bp = NEXT_BLKP(bp);

        PUT(HDRP(bp),PACK(csize-asize,0));
        PUT(FTRP(bp),PACK(csize-asize,0));

        PUT(NEXT_LINKNODE_RP(bp),0);
        PUT(PREV_LINKNODE_RP(bp),0);
        coalesce(bp);
    }
    else
    {
        PUT(HDRP(bp),PACK(csize,1));
        PUT(FTRP(bp),PACK(csize,1));
    }
}

/* ■■■■                                                                 ■■■■■■ */
/* ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ */




/* ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ *
 * Implementation of mm_check()
 * ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ */
#ifdef MMCHECK
int mm_check(void) {

    int c1 = CheckedFreeMarkedInFreeList();
    int c2 = checkEscapedCoalesce();
    int c3 = isEveryFreeBlockInFreeList();
    int c4 = checkPointerInFreeListValid();
    int c5 = checkPointerInHeapBlockValid();

    if (c1 > 0) {
        printf("\n\033[0;32;32m[MM_Checker]All free block in Free List are marked free.\033[0m\n");
    }
    if (c2 > 0) {
        printf("\n\033[0;32;32m[MM_Checker]No free block escaped coalesce.\033[0m\n");
    }
    if (c3 > 0) {
        printf("\n\033[0;32;32m[MM_Checker]Every block in the Free List.\033[0m\n");
    }
    if (c4 > 0) {
        printf("\n\033[0;32;32m[MM_Checker]Every Pointer in Free List is valid.\033[0m\n");
    }
    if (c5 > 0) {
        printf("\n\033[0;32;32m[MM_Checker]Every Pointer in Heap is valid.\033[0m\n");
    }


}

// Check every block in the free list marked free by forloop{} and GET_ALLOC()
int CheckedFreeMarkedInFreeList() {
	int i;
    void *p = mem_heap_lo();
	// void *p = heap_listp - 12 * WSIZE;

	int res = 1;

	for (i = 0; i < 10; i++) {
		void* tmpP = p + (i * WSIZE);

		// printf("\n\neach pointer %p \n", tmpP);
		// printf("each pointer %p \n", GET(tmpP));
		if (GET(tmpP) == NULL) {
			// SKIP empty item in list
		} else {
			size_t  head_alloc = GET_ALLOC(GET(tmpP));
			// printf("\n\n\nwhether allocated %d \n", head_alloc);

			if (!head_alloc) {
				// printf("No.%d is marked free.\n", i+1);
			}else {
				printf("No.%d didn't' marked free!\n", i+1);
				res = 0;
			}
		}
	}

	return res;
}

// check whether one free block's contiguous block is free
int checkEscapedCoalesce() {
	void* p = heap_listp + 8;
    void* p2 = mem_heap_hi();
	int res = 1;

	void* lastP = NULL;

	while (p) {
		size_t  head_alloc = GET_ALLOC(HDRP(p));

		if (head_alloc) {
			// SKIP allocated block
		}else {
			if (lastP != NULL && NEXT_BLKP(lastP) == p) {
				res = 0;
				printf("%p and %p is didn't coalesced yet!", lastP, p);
			}else {
                // printf("1111");
            }

			lastP = p;
		}
		p = NEXT_BLKP(p);

		if (p > p2) {
			p = NULL;
		}
	}

	return res;
}

// Check every block whether in free list
int isEveryFreeBlockInFreeList() {
	void* p = heap_listp + 8;
    void* p2 = mem_heap_hi();
	int res = 1;

	while (p) {
		size_t  head_alloc = GET_ALLOC(HDRP(p));

		if (head_alloc) {
			// SKIP allocated block
		}else {
			int i;
			int isExist = 0;
			for (i = 0; i < 10; i++) {
				void* tmpP = p + (i * WSIZE);

				if (&GET(tmpP) == p) {
					isExist = 1;
				}
			}
            
			if (isExist <= 0) {
				res = 0;
			}
		}
		p = NEXT_BLKP(p);

		if (p > p2) {
			p = NULL;
		}
	}

	return res;
}

int checkPointerInFreeListValid() {
    void *p = mem_heap_lo();
	// void *p = heap_listp - 12 * WSIZE;

	int res = 1;

	int i;
	for (i = 0; i < 10; i++) {
		void* tmpP = p + (i * WSIZE);

		if (GET(tmpP) == NULL) {
			// SKIP empty item in list
		} else {
			size_t  head_size = GET_SIZE(HDRP(GET(tmpP)));
			// printf("\n\n\nwhether allocated %d \n", head_alloc);

			if (head_size > 0) {
				// if greater than 0 it's valid
			} else {
				res = 0;
			}
		}
	}

    return res;
}

// check the pointer whether it is smaller than last byte and bigger than first byte.
int checkPointerInHeapBlockValid() {
    void* p1 = mem_heap_lo();
	void* p2 = mem_heap_hi();

    int res = 1;

	int i;
	for (i = 0; i < 10; i++) {
		void* tmpP = p1 + (i * WSIZE);

		if (GET(tmpP) == NULL) {
			// SKIP empty item in list
		} else {
            
			if (GET(tmpP) > p1 && GET(tmpP) < p2) {
                
            } else {
                res = 0;
            }
		}
	}

    return res;
}
#endif
/* ■■■■                                                                 ■■■■■■ */
/* ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ */
