#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "src/config/config.h"
#include "src/json/btc-e_ticker.h"
#include "src/nbus/nbus.h"
#include "src/url/url.h"
#include "src/utils/args.h"
#include "src/utils/daemon.h"
#include "src/utils/data.h"
#include "src/utils/errors.h"
#include "src/utils/logs.h"

#define APP_TAG "flip_crawler"

int main(int argc, char *argv[])
{
    TALLOC_CTX *mem_ctx;
    struct worker_args_ctx *args;
    struct config_ctx *config_ctx;
    struct url_conn_ctx *url_conn_ctx;
    struct nbus_ctx *nbus_ctx;
    struct string_ctx *chunk;
    struct btce_ticker *ticker_data;
    int updated = 0;
    errno_t ret;

    mem_ctx = talloc_new(NULL);
    if (mem_ctx == NULL) {
        LOG(LOG_CRIT, "Critical failure: Not enough memory.");
        exit(EXIT_FAILURE);
    }

    ret = parse_worker_args(mem_ctx, argc, argv, CRAWLER, &args);
    if (ret != EOK) {
        LOG(LOG_CRIT, "Critical failure: parse_worker_args() failed.");
        exit(EXIT_FAILURE);
    }

#ifdef DEBUG
    log_init(APP_TAG);
#else
    run_daemon(APP_TAG, config_ctx->work_dir);
#endif

    ret = nbus_init_pub(mem_ctx, config_ctx->socket, &nbus_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "Critical failure: Not enough memory.");
        exit(EXIT_FAILURE);
    }

    ret = url_global_init();
    if (ret != EOK) {
        LOG(LOG_CRIT, "curl init failed");
        exit(EXIT_FAILURE);
    }

    ret = url_init_ctx(mem_ctx, "https://btc-e.com/api/3/ticker/btc_usd",
                       &url_conn_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "Critical failure: Not enough memory.");
        exit(EXIT_FAILURE);
    }

    int i = 10;
    while (i > 0) {
        ret = url_get_data(mem_ctx, url_conn_ctx, &chunk);
        if (ret != EOK) {
            LOG(LOG_CRIT, "Critical failure: Not enough memory.");
            exit(EXIT_FAILURE);
        }

        ret = parse_btc_e_ticker(mem_ctx, chunk->data, &ticker_data);
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

        talloc_free(ticker_data);
        ticker_data = NULL;

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

    talloc_free(mem_ctx);

    exit(EXIT_SUCCESS);
}
