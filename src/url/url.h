#ifndef __URL_H__
#define __URL_H__

#include <stddef.h>

#include <curl/curl.h>
#include <talloc.h>

#include "src/utils/errors.h"
#include "src/utils/data.h"

errno_t url_global_init();
errno_t url_global_cleanup();

struct url_conn_ctx;

errno_t url_init_ctx(TALLOC_CTX* mem_ctx, const char* url,
                     struct url_conn_ctx** _url_conn_ctx);

errno_t url_get_data(TALLOC_CTX* mem_ctx, struct url_conn_ctx* url_conn_ctx,
                     struct string_ctx** _chunk);

errno_t url_close(struct url_conn_ctx* url_conn_ctx_ctx);

#endif /* __URL_H__ */