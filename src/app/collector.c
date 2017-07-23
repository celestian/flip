#include <stdlib.h>
#include <unistd.h>

#include <talloc.h>

#include "src/common/json/btc-e_ticker.h"
#include "src/common/msg/msg.h"
#include "src/common/nbus/nbus.h"
#include "src/common/sql/sql.h"
#include "src/common/utils/args.h"
#include "src/common/utils/daemon.h"
#include "src/common/utils/logs.h"

int main(int argc, char **argv)
{
    struct main_context *main_ctx = NULL;
    struct worker_args_ctx *args;
    struct nbus_ctx *root_nbus_ctx = NULL;
    struct nbus_ctx *nbus_ctx;
    struct sql_ctx *sql_ctx;
    struct string_ctx *chunk;
    struct btce_ticker *ticker_data;
    errno_t ret;

    ret = init_main_context(COLLECTOR, &main_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "Critical failure: init_main_context() failed.");
        ret = EXIT_FAILURE;
        goto done;
    }

    ret = parse_worker_args(main_ctx, argc, argv, COLLECTOR, &args);
    if (ret != EOK) {
        LOG(LOG_CRIT, "Critical failure: parse_worker_args() failed.");
        ret = EXIT_FAILURE;
        goto done;
    }

#ifndef DEBUG
    become_daemon(args->pid_file);
#endif

    ret = nbus_init_pair(main_ctx, args->root_ipc, &root_nbus_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "nbus_init_pair() failed.");
        ret = EXIT_FAILURE;
        goto done;
    }

    sleep(30);
    ret = EXIT_SUCCESS;
    goto done;

    // TODO zde nema byt args->root_ipc !!
    ret = nbus_init_sub(main_ctx, args->root_ipc, &nbus_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "Critical failure: nbus_init_sub() failed.");
        talloc_zfree(main_ctx);
        exit(EXIT_FAILURE);
    }

    // TODO zde nema byt args->root_ipc !!
    ret = sql_init(main_ctx, args->root_ipc, &sql_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "Critical failure: sql_init() failed.");
        talloc_zfree(main_ctx);
        exit(EXIT_FAILURE);
    }

    ret = sql_create_ticks_table(sql_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "Critical failure: sql_create_ticks_table() failed.");
        talloc_zfree(main_ctx);
        exit(EXIT_FAILURE);
    }

    int i = 20;
    while (i > 0) {
        ret = nbus_recieve(main_ctx, nbus_ctx, &chunk);
        if (ret != EOK && ret != EAGAIN) {
            LOG(LOG_CRIT, "Critical failure: nbus_recieve() failed.");
            talloc_zfree(main_ctx);
            exit(EXIT_FAILURE);
        }
        if (ret == EAGAIN) {
            sleep(1);
            i--;
            continue;
        }

        ret = parse_btc_e_ticker(main_ctx, chunk->data, &ticker_data);
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

    if (main_ctx != NULL) {
        talloc_zfree(main_ctx);
    }

    exit(ret);
}
