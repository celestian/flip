#include <stdlib.h>
#include <unistd.h>

#include <talloc.h>

#include "src/json/btc-e_ticker.h"
#include "src/msg/msg.h"
#include "src/nbus/nbus.h"
#include "src/sql/sql.h"
#include "src/utils/args.h"
#include "src/utils/daemon.h"
#include "src/utils/errors.h"
#include "src/utils/logs.h"

int main(int argc, char **argv)
{
    TALLOC_CTX *mem_ctx = NULL;
    struct worker_args_ctx *args;
    struct nbus_ctx *root_nbus_ctx = NULL;
    struct nbus_ctx *nbus_ctx;
    struct sql_ctx *sql_ctx;
    struct string_ctx *chunk;
    struct btce_ticker *ticker_data;
    errno_t ret;

    log_init(COLLECTOR);

    mem_ctx = talloc_new(NULL);
    if (mem_ctx == NULL) {
        LOG(LOG_CRIT, "Critical failure: Not enough memory.");
        ret = EXIT_FAILURE;
        goto done;
    }

    ret = parse_worker_args(mem_ctx, argc, argv, COLLECTOR, &args);
    if (ret != EOK) {
        LOG(LOG_CRIT, "Critical failure: parse_worker_args() failed.");
        ret = EXIT_FAILURE;
        goto done;
    }

#ifndef DEBUG
    run_daemon(args->pid_file);
#endif

    ret = nbus_init_pair(mem_ctx, args->root_ipc, &root_nbus_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "nbus_init_pair() failed.");
        ret = EXIT_FAILURE;
        goto done;
    }

    sleep(30);
    ret = EXIT_SUCCESS;
    goto done;

    // TODO zde nema byt args->root_ipc !!
    ret = nbus_init_sub(mem_ctx, args->root_ipc, &nbus_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "Critical failure: nbus_init_sub() failed.");
        talloc_zfree(mem_ctx);
        exit(EXIT_FAILURE);
    }

    // TODO zde nema byt args->root_ipc !!
    ret = sql_init(mem_ctx, args->root_ipc, &sql_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "Critical failure: sql_init() failed.");
        talloc_zfree(mem_ctx);
        exit(EXIT_FAILURE);
    }

    ret = sql_create_ticks_table(sql_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "Critical failure: sql_create_ticks_table() failed.");
        talloc_zfree(mem_ctx);
        exit(EXIT_FAILURE);
    }

    int i = 20;
    while (i > 0) {
        ret = nbus_recieve(mem_ctx, nbus_ctx, &chunk);
        if (ret != EOK && ret != EAGAIN) {
            LOG(LOG_CRIT, "Critical failure: nbus_recieve() failed.");
            talloc_zfree(mem_ctx);
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

        talloc_zfree(ticker_data);
        talloc_zfree(chunk);

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

    ret = EXIT_SUCCESS;

done:
    stop_daemon();

    if (root_nbus_ctx != NULL) {
        ret = nbus_close(root_nbus_ctx);
        if (ret != EOK) {
            LOG(LOG_CRIT, "nbus_close() failed.");
            ret = EXIT_FAILURE;
        }
    }

    if (mem_ctx != NULL) {
        talloc_zfree(mem_ctx);
    }

    exit(ret);
}
