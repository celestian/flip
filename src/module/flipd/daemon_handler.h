#ifndef __DAEMON_HANDLER__
#define __DAEMON_HANDLER__

#include <talloc.h>
#include <tevent.h>

#include "src/nbus/nbus.h"
#include "src/utils/errors.h"

errno_t get_answer(TALLOC_CTX *mem_ctx, struct nbus_ctx *nbus_ctx);

#endif /* __DAEMON_HANDLER__ */