#define _GNU_SOURCE

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <systemd/sd-journal.h>
#include <unistd.h>

#include "src/utils/logs.h"

#define DEFAULT_APP_TAG "flip"
#define FLIPD_APP_TAG "flipd"
#define CRAWLER_APP_TAG "flip_crawler"
#define COLLECTOR_APP_TAG "flip_collector"

static const char *static_variable_app_tag;

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
                    "SYSLOG_IDENTIFIER=%s", static_variable_app_tag,
                    "CODE_FILE=%s", file, "CODE_LINE=%d", line, "CODE_FUNC=%s",
                    function, "ERRNO=%d", errno, "ERRNO_STR=%s",
                    strerror(errno), NULL);

#ifdef DEBUG
    printf("[%s::%i | %s:%d] [%s] %s\n", static_variable_app_tag, priority,
           file, line, function, message);
#endif

    free(message);
}

void log_init(enum daemon_type daemon_type)
{
    switch (daemon_type) {
    case FLIPD:
        static_variable_app_tag = FLIPD_APP_TAG;
        break;
    case CRAWLER:
        static_variable_app_tag = CRAWLER_APP_TAG;
        break;
    case COLLECTOR:
        static_variable_app_tag = COLLECTOR_APP_TAG;
        break;
    default:
        static_variable_app_tag = DEFAULT_APP_TAG;
    }
}

void log_fn(const char *function, const char *file, int line, int priority,
            const char *format, ...)
{
    va_list ap;

    va_start(ap, format);
    log_handler(function, file, line, priority, format, ap);
    va_end(ap);
}
