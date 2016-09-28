#include <stdlib.h>
#include <stdio.h>
#define UNW_LOCAL_ONLY
#include <libunwind.h>



int get_callinfo(char *fname, size_t fnlen, unsigned long long *ofs)
{

    printf("\nname:%s  length:%i \n", fname, fnlen);

  return 1;
}


void show_backtrace (void) {
    unw_cursor_t cursor;
	unw_context_t uc;
    unw_word_t ip, sp;

	unw_getcontext(&uc);
	unw_init_local(&cursor, &uc);
 	while (unw_step(&cursor) > 0) {
   		unw_get_reg(&cursor, UNW_REG_IP, &ip);
 		unw_get_reg(&cursor, UNW_REG_SP, &sp);
		printf ("ip = %lx, sp = %lx\n", (long) ip, (long) sp);
	}
}