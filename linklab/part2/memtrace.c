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
/* My code for function implementation
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

	n_malloc += size;
	alloc_times++;


	alloc(list, ptr, size);

	return ptr;
}

void free(void *p) {

	if (!freep) {
		// void (*freep)(void *ptr)
		freep = /*(void (*)(void *))*/dlsym(RTLD_NEXT, "free");
	}

	LOG_FREE(p);
	
	// free_size++;
	dealloc(list, p);
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