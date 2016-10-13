#include <stdio.h>
#include "debug.h"
#include "function.h"


/* ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ *
 * Implimentation of the function
 * ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ */
/*********************************************************
 * Auxiliary Function 
 ********************************************************/
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
    else i= 7;
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

    if((long)(bp = mem_sbrk(size))==(void *)-1)		/*move the brk pointer for bigger heap*/
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
    }
    else{
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


/* ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ */