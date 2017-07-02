#define _GNU_SOURCE

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <systemd/sd-journal.h>
#include <unistd.h>

#include "src/utils/logs.h"

static const char *tag;

static void log_handler(const char *function, const char *file, int line,
                        int priority, const char *format, va_list ap)
{
    char *message;
    int ret;

    ret = vasprintf(&message, format, ap);
    if (ret == -1) {
        /* ENOMEM */
        return;
    }

    sd_journal_send("MESSAGE=%s", message, "PRIORITY=%i", priority,
                    "SYSLOG_IDENTIFIER=%s", tag, "CODE_FILE=%s", file,
                    "CODE_LINE=%d", line, "CODE_FUNC=%s", function, "ERRNO=%d",
                    errno, "ERRNO_STR=%s", strerror(errno), NULL);

#ifdef DEBUG
    printf("[%s::%i | %s:%d] [%s] %s [errno (%d) %s]\n", tag, priority, file,
           line, function, message, errno, strerror(errno));
#endif

    free(message);
}

void log_init(const char *identity_tag) { tag = identity_tag; }

void log_fn(const char *function, const char *file, int line, int priority,
            const char *format, ...)
{
    va_list ap;

    va_start(ap, format);
    log_handler(function, file, line, priority, format, ap);
    va_end(ap);
}
