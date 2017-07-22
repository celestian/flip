#ifndef __UTIL_LOGS_H__
#define __UTIL_LOGS_H__

#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include <syslog.h>

#include "src/common/utils/utils.h"

#ifndef EOK
#define EOK 0
#endif

#ifndef HAVE_ERRNO_T
#define HAVE_ERRNO_T
typedef int errno_t;
#endif

/*
    JOURNALD:
    --------------------------------------------------------------------------
    Kernel constant   Level value   Meaning
    LOG_EMERG             0        System is unusable
    LOG_ALERT             1        Action must be taken immediately
    LOG_CRIT              2        Critical conditions
    LOG_ERR               3        Error conditions
    LOG_WARNING           4        Warning conditions
    LOG_NOTICE            5        Normal but significant condition
    LOG_INFO              6        Informational
    LOG_DEBUG             7        Debug-level messages
 */

#define LOG(level, format, ...)                                                \
    do {                                                                       \
        log_fn(__FUNCTION__, __FILE__, __LINE__, level, format,                \
               ##__VA_ARGS__);                                                 \
    } while (0);

void log_init(enum daemon_type daemon_type);

void log_fn(const char *function, const char *file, int line, int priority,
            const char *format, ...);

#endif /* __UTIL_LOGS_H__ */
