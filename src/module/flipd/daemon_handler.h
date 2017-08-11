#ifndef __DAEMON_HANDLER__
#define __DAEMON_HANDLER__

#include <talloc.h>
#include <tevent.h>

#include "src/common/nbus/nbus.h"
#include "src/common/utils/data.h"
#include "src/common/utils/logs.h"

struct worker_daemon_ctx {
    const char *identity_name;
    const char *pid_file;
    const char *url_root_ipc;

    struct nbus_ctx *nbus_ctx;
};

errno_t async_start_worker_daemon2(TALLOC_CTX *mem_ctx,
                                   struct worker_daemon_ctx *wd_ctx,
                                   struct string_ctx **chunk);

errno_t async_start_worker_daemon(TALLOC_CTX *mem_ctx,
                                  const char *identity_name,
                                  const char *pid_file,
                                  const char *url_root_ipc);

#endif /* __DAEMON_HANDLER__ */