#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "src/nbus/nbus.h"
#include "src/url/url.h"
#include "src/utils/daemon.h"
#include "src/utils/data.h"
#include "src/utils/errors.h"
#include "src/utils/logs.h"

int main(int argc, char* argv[])
{
    TALLOC_CTX* mem_ctx;
    struct url_conn_ctx* url_conn_ctx;
    struct nbus_ctx* nbus_ctx;
    struct string_ctx* chunk;
    errno_t ret;

    run_daemon("flip_crawler");

    mem_ctx = talloc_new(NULL);
    if (mem_ctx == NULL) {
        LOG(LOG_CRIT, "Critical failure: Not enough memory.");
        exit(EXIT_FAILURE);
    }

    LOG(LOG_CRIT, "Test A.");

    ret = nbus_init(mem_ctx, "ipc:///tmp/listener_pubsub.ipc", &nbus_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "Critical failure: Not enough memory.");
        exit(EXIT_FAILURE);
    }

    ret = url_global_init();

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

        ret = nbus_send(nbus_ctx, chunk->data, chunk->size);
        if (ret != EOK) {
            LOG(LOG_CRIT, "Critical failure: Not enough memory.");
            exit(EXIT_FAILURE);
        }

        sleep(1);
        i--;
    }

    ret = nbus_close(nbus_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "Critical failure: Not enough memory.");
        exit(EXIT_FAILURE);
    }

    ret = url_global_cleanup();

    talloc_free(mem_ctx);

    exit(EXIT_SUCCESS);
}
