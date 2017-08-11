#ifndef __LISTEN_HANDLER__
#define __LISTEN_HANDLER__

#include <talloc.h>
#include <tevent.h>

#include "src/common/nbus/nbus.h"
#include "src/common/utils/logs.h"

errno_t async_listen(TALLOC_CTX *mem_ctx, struct tevent_context *ev,
                     struct nbus_ctx *nbus_ctx);

#endif /* __LISTEN_HANDLER__ */