#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <talloc.h>

#include "src/common/json/btc-e_ticker.h"
#include "src/common/nbus/nbus.h"
#include "src/common/url/url.h"
#include "src/common/utils/args.h"
#include "src/common/utils/daemon.h"
#include "src/common/utils/data.h"
#include "src/common/utils/logs.h"
#include "src/module/common_daemon.h"

int main(int argc, char *argv[])
{
    struct main_context *main_ctx = NULL;
    struct worker_args_ctx *args;
    struct url_conn_ctx *url_conn_ctx;
    struct nbus_ctx *root_nbus_ctx = NULL;
    struct nbus_ctx *nbus_ctx;
    struct string_ctx *chunk;
    struct btce_ticker *ticker_data;
    int updated = 0;
    errno_t ret;

    ret = init_main_context(CRAWLER, &main_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "Critical failure: init_main_context() failed.");
        ret = EXIT_FAILURE;
        goto done;
    }

    ret = parse_worker_args(main_ctx, argc, argv, CRAWLER, &args);
    if (ret != EOK) {
        LOG(LOG_CRIT, "Critical failure: parse_worker_args() failed.");
        ret = EXIT_FAILURE;
        goto done;
    }

#ifndef DEBUG
    run_daemon(args->pid_file);
#endif

    ret = nbus_init_pair(main_ctx, args->root_ipc, &root_nbus_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "nbus_init_pair() failed.");
        ret = EXIT_FAILURE;
        goto done;
    }

    // -------------------------------------------------------------------------

    char *config_json;

    ret = get_config_from_root_daemon(
        main_ctx, root_nbus_ctx, args->identity_name, CRAWLER, &config_json);
    if (ret != EOK) {
        LOG(LOG_CRIT, "get_config_from_root_daemon() failed.");
        ret = EXIT_FAILURE;
        goto done;
    }

    sleep(30);
    ret = EXIT_SUCCESS;
    goto done;

    // -------------------------------------------------------------------------

    // TODO zde nema byt args->root_ipc !!
    ret = nbus_init_pub(main_ctx, args->root_ipc, &nbus_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "Critical failure: Not enough memory.");
        exit(EXIT_FAILURE);
    }

    ret = url_global_init();
    if (ret != EOK) {
        LOG(LOG_CRIT, "curl init failed");
        exit(EXIT_FAILURE);
    }

    ret = url_init_ctx(main_ctx, "https://btc-e.com/api/3/ticker/btc_usd",
                       &url_conn_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "Critical failure: Not enough memory.");
        exit(EXIT_FAILURE);
    }

    int i = 10;
    while (i > 0) {
        ret = url_get_data(main_ctx, url_conn_ctx, &chunk);
        if (ret != EOK) {
            LOG(LOG_CRIT, "Critical failure: Not enough memory.");
            exit(EXIT_FAILURE);
        }

        ret = parse_btc_e_ticker(main_ctx, chunk->data, &ticker_data);
        if (ret != EOK) {
            LOG(LOG_CRIT, "Critical failure: Not enough memory.");
            exit(EXIT_FAILURE);
        }

        if (ticker_data->updated > updated) {
            updated = ticker_data->updated;
            ret = nbus_send(nbus_ctx, chunk->data, chunk->size);
            if (ret != EOK) {
                LOG(LOG_CRIT, "Critical failure: Not enough memory.");
                exit(EXIT_FAILURE);
            }
        }

        talloc_zfree(ticker_data);

        sleep(1);
        i--;
    }

    sleep(15);

    ret = nbus_close(nbus_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "Critical failure: Not enough memory.");
        exit(EXIT_FAILURE);
    }

    url_global_cleanup();

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
