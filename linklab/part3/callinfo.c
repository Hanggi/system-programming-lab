#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define UNW_LOCAL_ONLY
#include <libunwind.h>

char funcName[256];

static int aa = 0;

int get_callinfo(char *fname, size_t fnlen, unsigned long long *ofs)
{
    // printf("\n %d?.n:%s len:%p ofs:%lx \n", aa, fname, fnlen, ofs);
	// printf("test:", NULL),
	aa++;
	// printf("%d \n", aa);

	char name[32];
    unw_cursor_t cursor;
	unw_context_t uc;
    unw_word_t ip, sp, offp;

	unw_getcontext(&uc);
	unw_init_local(&cursor, &uc);

 	while (unw_step(&cursor) > 0) {
		int tmp = unw_get_proc_name(&cursor, name, 32, &offp);
		// printf("%d:%s\n", tmp, name);
   		unw_get_reg(&cursor, UNW_REG_IP, &ip);
 		unw_get_reg(&cursor, UNW_REG_SP, &sp);
		// printf ("ip = %lx, sp = %lx\n", (long) ip, (long) sp);

		// printf("%s in file 0x%lx \n", name, offp);
		char* mm = "main";
		// printf("")
		if (strcmp(name, mm) == 0) {
			// strcmp(name, mm)
			// *fname = name;
			// printf("name: %s\n", fname);
			strcpy(fname, name);
			*ofs = offp;
		}
	}
    // printf("\n %d!.n:%s len:%p ofs:%lx \n", aa, fname, fnlen, ofs);

	return 0;
}


void show_backtrace (char *fname, unsigned long long *ofs) {
	
}