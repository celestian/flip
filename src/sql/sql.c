#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <nanomsg/nn.h>
#include <nanomsg/pubsub.h>
#include <talloc.h>

#include "sql.h"
#include "sql_private.h"
#include "src/utils/errors.h"
#include "src/utils/logs.h"

errno_t sql_init(TALLOC_CTX *mem_ctx, const char *db_file_name,
                 struct sql_ctx **_sql_ctx)
{
    TALLOC_CTX *tmp_ctx;
    struct sql_ctx *sql_ctx = NULL;
    errno_t ret;

    tmp_ctx = talloc_new(NULL);
    if (tmp_ctx == NULL) {
        LOG(LOG_ERR, "talloc_new() failed.");
        return ENOMEM;
    }

    sql_ctx = talloc_zero(tmp_ctx, struct sql_ctx);
    if (sql_ctx == NULL) {
        LOG(LOG_ERR, "talloc_zero() failed.");
        ret = ENOMEM;
        goto done;
    }

    ret = sqlite3_open(db_file_name, &sql_ctx->db);
    if (ret != SQLITE_OK) {
        LOG(LOG_ERR, "sqlite3_open() failed [%s | %s]", db_file_name,
            sqlite3_errmsg(sql_ctx->db));
        sqlite3_close(sql_ctx->db);
        ret = EIO;
        goto done;
    }

    *_sql_ctx = talloc_steal(mem_ctx, sql_ctx);
    ret = EOK;

done:
    talloc_free(tmp_ctx);
    return ret;
}

errno_t sql_close(struct sql_ctx *sql_ctx)
{
    int ret;

    if (sql_ctx == NULL) {
        ret = EOK;
        goto done;
    }

    ret = sqlite3_close(sql_ctx->db);
    // if (ret != SQLITE_OK)

    talloc_free(sql_ctx);
    ret = EOK;

done:
    return ret;
}
