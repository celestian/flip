#ifndef __UTIL_UTILS_H__
#define __UTIL_UTILS_H__

#include <stdbool.h>
#include <stdint.h>

#ifndef discard_const
#define discard_const(ptr) ((void *)((uintptr_t)(ptr)))
#endif

#ifndef discard_const_p
#if defined(__intptr_t_defined) || defined(HAVE_INTPTR_T)
#define discard_const_p(type, ptr) ((type *)((intptr_t)(ptr)))
#else
#define discard_const_p(type, ptr) ((type *)(ptr))
#endif
#endif

#ifndef talloc_zfree
#define talloc_zfree(ptr)                                                      \
    do {                                                                       \
        talloc_free(discard_const(ptr));                                       \
        ptr = NULL;                                                            \
    } while (0)
#endif

enum daemon_type { FLIPD, CRAWLER, COLLECTOR };

bool is_file_exist(const char *file_name);

#endif /* __UTIL_UTILS_H__ */