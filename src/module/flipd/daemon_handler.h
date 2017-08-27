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

#endif /* __DAEMON_HANDLER__ */