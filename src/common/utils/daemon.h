#ifndef __DAEMON_H__
#define __DAEMON_H__

#include "src/common/utils/logs.h"
#include "src/common/utils/utils.h"

struct main_context {
    ;
};

errno_t init_main_context(enum daemon_type daemon_type,
                          struct main_context **_main_ctx);

void become_daemon(char *pid_file);
void stop_daemon();

#endif /* __DAEMON_H__ */