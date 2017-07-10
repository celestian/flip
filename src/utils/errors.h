#ifndef __UTIL_ERRORS_H__
#define __UTIL_ERRORS_H__

#include <errno.h>
#include <string.h>

#ifndef EOK
#define EOK 0
#endif

#ifndef HAVE_ERRNO_T
#define HAVE_ERRNO_T
typedef int errno_t;
#endif

#endif /* __UTIL_ERRORS_H__ */