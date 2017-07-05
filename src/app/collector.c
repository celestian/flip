#include <stdlib.h>
#include <unistd.h>

#include "config.h"
#include "src/config/config.h"
#include "src/json/btc-e_ticker.h"
#include "src/nbus/nbus.h"
#include "src/sql/sql.h"
#include "src/utils/args.h"
#include "src/utils/daemon.h"
#include "src/utils/errors.h"
#include "src/utils/logs.h"

#define APP_TAG "flip_collector"

int main(int argc, char **argv)
{
    TALLOC_CTX *mem_ctx;
    struct worker_args_ctx *args;
    struct config_ctx *config_ctx;
    struct nbus_ctx *nbus_ctx;
    struct sql_ctx *sql_ctx;
    struct string_ctx *chunk;
    struct btce_ticker *ticker_data;
    errno_t ret;

    mem_ctx = talloc_new(NULL);
    if (mem_ctx == NULL) {
        LOG(LOG_CRIT, "Critical failure: Not enough memory.");
        exit(EXIT_FAILURE);
    }

    ret = parse_worker_args(mem_ctx, argc, argv, COLLECTOR, &args);
    if (ret != EOK) {
        LOG(LOG_CRIT, "Critical failure: parse_worker_args() failed.");
        exit(EXIT_FAILURE);
    }

#ifdef DEBUG
    log_init(APP_TAG);
#else
    run_daemon(APP_TAG, config_ctx->work_dir);
#endif

    ret = nbus_init_sub(mem_ctx, config_ctx->socket, &nbus_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "Critical failure: nbus_init_sub() failed.");
        talloc_free(mem_ctx);
        exit(EXIT_FAILURE);
    }

    ret = sql_init(mem_ctx, config_ctx->db, &sql_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "Critical failure: sql_init() failed.");
        talloc_free(mem_ctx);
        exit(EXIT_FAILURE);
    }

    ret = sql_create_ticks_table(sql_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "Critical failure: sql_create_ticks_table() failed.");
        talloc_free(mem_ctx);
        exit(EXIT_FAILURE);
    }

    int i = 20;
    while (i > 0) {
        ret = nbus_recieve(mem_ctx, nbus_ctx, &chunk);
        if (ret != EOK && ret != EAGAIN) {
            LOG(LOG_CRIT, "Critical failure: nbus_recieve() failed.");
            talloc_free(mem_ctx);
            exit(EXIT_FAILURE);
        }
        if (ret == EAGAIN) {
            sleep(1);
            i--;
            continue;
        }

        ret = parse_btc_e_ticker(mem_ctx, chunk->data, &ticker_data);
        if (ret != EOK) {
            LOG(LOG_CRIT, "Critical failure: parse_btc_e_ticker() failed.");
            exit(EXIT_FAILURE);
        }

        ret = sql_insert_tick(sql_ctx, ticker_data);
        if (ret != EOK) {
            LOG(LOG_CRIT, "Critical failure: sql_insert_tick() failed.");
            exit(EXIT_FAILURE);
        }

        talloc_free(ticker_data);
        ticker_data = NULL;

        talloc_free(chunk);
        chunk = NULL;

        sleep(1);
        i--;
    }

    ret = sql_close(sql_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "Critical failure: sql_close() failed.");
        exit(EXIT_FAILURE);
    }

    ret = nbus_close(nbus_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "Critical failure: Not enough memory.");
        exit(EXIT_FAILURE);
    }

    talloc_free(mem_ctx);

    exit(EXIT_SUCCESS);
}
