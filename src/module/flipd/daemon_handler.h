#ifndef __DAEMON_HANDLER__
#define __DAEMON_HANDLER__

#include <talloc.h>
#include <tevent.h>

#include "src/nbus/nbus.h"
#include "src/utils/data.h"
#include "src/utils/errors.h"

struct worker_deamon_ctx {
    const char *identity_name;
    const char *pid_file;
    const char *root_ipc;

    struct nbus_ctx *nbus_ctx;
};

errno_t async_start_worker_daemon(TALLOC_CTX *mem_ctx,
                                  struct worker_deamon_ctx *wd_ctx,
                                  struct string_ctx **chunk);

#endif /* __DAEMON_HANDLER__ */