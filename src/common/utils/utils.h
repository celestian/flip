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

#define TEVENT_REQ_RETURN_ON_ERROR(req)                                        \
    do {                                                                       \
        enum tevent_req_state TRROEstate;                                      \
        uint64_t TRROEuint64;                                                  \
        errno_t TRROEerr;                                                      \
                                                                               \
        if (tevent_req_is_error(req, &TRROEstate, &TRROEuint64)) {             \
            TRROEerr = (errno_t)TRROEuint64;                                   \
            if (TRROEstate == TEVENT_REQ_USER_ERROR) {                         \
                if (TRROEerr == 0) {                                           \
                    return EFAULT;                                             \
                }                                                              \
                return TRROEerr;                                               \
            }                                                                  \
            return EFAULT;                                                     \
        }                                                                      \
    } while (0)

enum daemon_type { FLIP, FLIPD, CRAWLER, COLLECTOR };

bool is_file_exist(const char *file_name);

#endif /* __UTIL_UTILS_H__ */