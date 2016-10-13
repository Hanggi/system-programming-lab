

#include "memlib.h"

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

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

/*aux function for mm_realloc*/
static void *realloc_coalesce(void *bp,size_t newSize,int *isNextFree);
static void realloc_place(void *bp,size_t asize);

static char *heap_listp = NULL;
static char *block_list_start = NULL;