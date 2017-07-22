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
#include "src/common/utils/logs.h"

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
        sql_close(sql_ctx);
        ret = EIO;
        goto done;
    }

    *_sql_ctx = talloc_steal(mem_ctx, sql_ctx);
    ret = EOK;

done:
    talloc_zfree(tmp_ctx);
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
    if (ret != SQLITE_OK) {
        ret = EIO;
        goto done;
    }

    talloc_zfree(sql_ctx);
    ret = EOK;

done:
    return ret;
}

errno_t sql_create_ticks_table(struct sql_ctx *sql_ctx)
{
    char *err_msg = 0;
    errno_t ret;

    char *sql = "CREATE TABLE IF NOT EXISTS Ticks ("
                "    id INT,"
                "    pair TEXT,"
                "    high REAL,"
                "    low REAL,"
                "    avg REAL,"
                "    vol REAL,"
                "    vol_cur REAL,"
                "    last REAL,"
                "    buy REAL,"
                "    sell REAL,"
                "    updated NUMERIC,"
                "    PRIMARY KEY(id ASC)"
                ")";

    ret = sqlite3_exec(sql_ctx->db, sql, 0, 0, &err_msg);
    if (ret != SQLITE_OK) {
        LOG(LOG_ERR, "sqlite3_exec() failed [%s]", err_msg);
        sqlite3_free(err_msg);
        sql_close(sql_ctx);
        ret = EIO;
        goto done;
    }

    ret = EOK;

done:
    return ret;
}

errno_t sql_insert_tick(struct sql_ctx *sql_ctx, struct btce_ticker *data)
{
    errno_t ret;
    sqlite3_stmt *statement;

    const char *sql = "INSERT INTO Ticks"
                      "(pair, high, low, avg, vol, vol_cur,"
                      " last, buy, sell, updated)"
                      "VALUES (?,?,?,?,?,?,?,?,?,?)";

    ret = sqlite3_prepare_v2(sql_ctx->db, sql, -1, &statement, NULL);
    if (ret != SQLITE_OK) {
        LOG(LOG_ERR, "sqlite3_prepare_v2() failed [%s]",
            sqlite3_errmsg(sql_ctx->db));
    }

    ret = sqlite3_bind_text(statement, 1, data->pair, strlen(data->pair), NULL);
    if (ret != SQLITE_OK) {
        LOG(LOG_ERR, "sqlite3_bind_text() failed [%s]",
            sqlite3_errmsg(sql_ctx->db));
        ret = EINVAL;
        goto done;
    }

    ret = sqlite3_bind_double(statement, 2, data->high);
    if (ret != SQLITE_OK) {
        LOG(LOG_ERR, "sqlite3_bind_double() failed [%s]",
            sqlite3_errmsg(sql_ctx->db));
        ret = EINVAL;
        goto done;
    }

    ret = sqlite3_bind_double(statement, 3, data->low);
    if (ret != SQLITE_OK) {
        LOG(LOG_ERR, "sqlite3_bind_double() failed [%s]",
            sqlite3_errmsg(sql_ctx->db));
        ret = EINVAL;
        goto done;
    }

    ret = sqlite3_bind_double(statement, 4, data->avg);
    if (ret != SQLITE_OK) {
        LOG(LOG_ERR, "sqlite3_bind_double() failed [%s]",
            sqlite3_errmsg(sql_ctx->db));
        ret = EINVAL;
        goto done;
    }

    ret = sqlite3_bind_double(statement, 5, data->vol);
    if (ret != SQLITE_OK) {
        LOG(LOG_ERR, "sqlite3_bind_double() failed [%s]",
            sqlite3_errmsg(sql_ctx->db));
        ret = EINVAL;
        goto done;
    }

    ret = sqlite3_bind_double(statement, 6, data->vol_cur);
    if (ret != SQLITE_OK) {
        LOG(LOG_ERR, "sqlite3_bind_double() failed [%s]",
            sqlite3_errmsg(sql_ctx->db));
        ret = EINVAL;
        goto done;
    }

    ret = sqlite3_bind_double(statement, 7, data->last);
    if (ret != SQLITE_OK) {
        LOG(LOG_ERR, "sqlite3_bind_double() failed [%s]",
            sqlite3_errmsg(sql_ctx->db));
        ret = EINVAL;
        goto done;
    }

    ret = sqlite3_bind_double(statement, 8, data->buy);
    if (ret != SQLITE_OK) {
        LOG(LOG_ERR, "sqlite3_bind_double() failed [%s]",
            sqlite3_errmsg(sql_ctx->db));
        ret = EINVAL;
        goto done;
    }

    ret = sqlite3_bind_double(statement, 9, data->sell);
    if (ret != SQLITE_OK) {
        LOG(LOG_ERR, "sqlite3_bind_double() failed [%s]",
            sqlite3_errmsg(sql_ctx->db));
        ret = EINVAL;
        goto done;
    }

    ret = sqlite3_bind_int(statement, 10, data->updated);
    if (ret != SQLITE_OK) {
        LOG(LOG_ERR, "sqlite3_bind_int() failed [%s]",
            sqlite3_errmsg(sql_ctx->db));
        ret = EINVAL;
        goto done;
    }

    ret = sqlite3_step(statement);
    if (ret != SQLITE_DONE) {
        LOG(LOG_ERR, "sqlite3_step() failed [%s]", sqlite3_errmsg(sql_ctx->db));
        ret = EIO;
        goto done;
    }

    ret = sqlite3_finalize(statement);
    if (ret != SQLITE_DONE) {
        LOG(LOG_ERR, "sqlite3_finalize() failed [%s]",
            sqlite3_errmsg(sql_ctx->db));
        ret = EIO;
        goto done;
    }

    ret = EOK;

done:
    return ret;
}
