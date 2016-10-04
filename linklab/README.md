# linklab

### memtrace

在程序运行中获取地址分配信息、释放信息、未释放内容、调用函数与偏移量、阻止多次释放和非法释放。

关键函数

```c
mallocp = /*(void *(*)(size_t))*/dlsym(RTLD_NEXT, "malloc");
```

libunwind
```c
#define UNW_LOCAL_ONLY
#include <libunwind.h>

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
```
>int unw_get_proc_name(unw_cursor_t *, char *, size_t, unw_word_t *);   
