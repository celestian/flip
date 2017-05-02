#ifndef __NBUS_H__
#define __NBUS_H__

#include <stddef.h>
#include <talloc.h>

#include "src/utils/errors.h"

struct nbus_ctx;

errno_t nbus_init(TALLOC_CTX* mem_ctx, const char* url,
                  struct nbus_ctx** _nbus_ctx);

errno_t nbus_close(struct nbus_ctx* nbus_ctx);

errno_t nbus_send(struct nbus_ctx* nbus_ctx, const char* msg, size_t msg_size);

#endif /* __NBUS_H__ */