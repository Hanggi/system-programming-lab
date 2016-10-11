//------------------------------------------------------------------------------
//
// memtrace
//
// trace calls to the dynamic memory manager
//
#define _GNU_SOURCE

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <memlog.h>
#include <memlist.h>
#include "callinfo.h"

//
// function pointers to stdlib's memory management functions
//
static void *(*mallocp)(size_t size) = NULL;
static void (*freep)(void *ptr) = NULL;
static void *(*callocp)(size_t nmemb, size_t size);
static void *(*reallocp)(void *ptr, size_t size);

//
// statistics & other global variables
//
static unsigned long n_malloc  = 0;
static unsigned long n_calloc  = 0;
static unsigned long n_realloc = 0;
static unsigned long n_allocb  = 0;
static unsigned long n_freeb   = 0;
static item *list = NULL;

/* ****************************************************************************
/* My code definitions
/* ****************************************************************************/
static int alloc_times = 0;
static int free_size = 0;
void show_bloks(item *list);
/******************************************************************************/

//
// init - this function is called once when the shared library is loaded
//
__attribute__((constructor))
void init(void)
{
  char *error;

  LOG_START();

  // initialize a new list to keep track of all memory (de-)allocations
  // (not needed for part 1)
  list = new_list();


  // ...
/* ****************************************************************************
/* My code in init
/* ****************************************************************************/

	// free_list(list);

/******************************************************************************/
}

//
// fini - this function is called once when the shared library is unloaded
//
__attribute__((destructor))
void fini(void)
{
  // ...

  LOG_STATISTICS(n_malloc, n_malloc/alloc_times, free_size);


/* ****************************************************************************
/* My code in fini
/* ****************************************************************************/
  	LOG_NONFREED_START();

	// dump_list(list);
	show_bloks(list);
/******************************************************************************/

  LOG_STOP();

  // free list (not needed for part 1)
  free_list(list);
}

// ...

/* ****************************************************************************
/* My code - malloc/realloc/calloc/free
/* ****************************************************************************/
void *malloc(size_t size)  
{  
	if (!mallocp) {
		// void *(*mallocp)(size_t size)
		mallocp = /*(void *(*)(size_t))*/dlsym(RTLD_NEXT, "malloc");
	}

    void *ptr;
    ptr = mallocp(size);
	LOG_MALLOC(size, ptr);

	// 计数
	n_malloc += size;
	alloc_times++;

	// 列表
	alloc(list, ptr, size);

	return ptr;
}

void *realloc(void *p, size_t size)  
{  
	if (!reallocp) {
		// void *(*mallocp)(size_t size)
		reallocp = /*(void *(*)(size_t))*/dlsym(RTLD_NEXT, "realloc");
	}

    void *ptr;  
    ptr = reallocp(p, size);
	LOG_REALLOC(p, size, ptr);

	// get last alloc size
	item *tempp = find(list, p);
	// n_malloc -= tempp->size;
	free_size += tempp->size;

	// dealloc last p and alloc new ptr, for list
	dealloc(list, p);
	alloc(list, ptr, size);

	// count
	n_malloc += size;
	alloc_times++;

	return ptr;
}

void *calloc(size_t nmemb, size_t size)  
{  
	if (!callocp) {
		// void *(*mallocp)(size_t size)
		callocp = /*(void *(*)(size_t))*/dlsym(RTLD_NEXT, "calloc");
	}

    void *ptr;  
    ptr = callocp(nmemb, size);
	LOG_CALLOC(nmemb, size, ptr);

	// list alloc
	alloc(list, ptr, size);

	// count
	n_malloc += size;
	alloc_times++;

	return ptr;
}

void free(void *p) {
	if (!freep) {
		// void (*freep)(void *ptr)
		freep = /*(void (*)(void *))*/dlsym(RTLD_NEXT, "free");
	}

	// list dealloc
	dealloc(list, p);

	LOG_FREE(p);
	
	// count the size of free
	item *tempp = find(list, p);
	free_size += tempp->size;

	freep(p);
}

void show_bloks(item *list) {
//   assert(list != NULL);
	item *i = list->next;

	while (i != NULL) {
		if (i->cnt > 0) {
			LOG_BLOCK(i->ptr, i->size, i->cnt, "Nothing", i->ofs);
		}
    i = i->next;
  }
}
/******************************************************************************/