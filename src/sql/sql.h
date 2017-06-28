#ifndef __SQL_H__
#define __SQL_H__

#include <stddef.h>
#include <talloc.h>

#include "src/utils/errors.h"

struct sql_ctx;

errno_t sql_init(TALLOC_CTX *mem_ctx, const char *db_file_name,
                 struct sql_ctx **_sql_ctx);

errno_t sql_close(struct sql_ctx *sql_ctx);

errno_t sql_create_ticks_table(struct sql_ctx *sql_ctx);

#endif /* __SQL_H__ */