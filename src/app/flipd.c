#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <talloc.h>

#include "src/common/msg/msg.h"
#include "src/common/nbus/nbus.h"
#include "src/common/url/url.h"
#include "src/common/utils/args.h"
#include "src/common/utils/conf.h"
#include "src/common/utils/daemon.h"
#include "src/common/utils/data.h"
#include "src/common/utils/logs.h"
#include "src/module/flipd/daemon_handler.h"

int main(int argc, char *argv[])
{
    TALLOC_CTX *mem_ctx = NULL;
    struct flipd_args_ctx *args;
    struct config_ctx *config_ctx;
    struct nbus_ctx *io_nbus_ctx = NULL;
    struct string_ctx *chunk;
    errno_t ret;

    log_init(FLIPD);

    mem_ctx = talloc_new(talloc_autofree_context());
    if (mem_ctx == NULL) {
        LOG(LOG_CRIT, "Critical failure: Not enough memory.");
        ret = EXIT_FAILURE;
        goto done;
    }

    ret = parse_flipd_args(mem_ctx, argc, argv, &args);
    if (ret != EOK) {
        LOG(LOG_CRIT, "Critical failure: parse_flipd_args() failed.");
        ret = EXIT_FAILURE;
        goto done;
    }

    ret = parse_flipd_config(mem_ctx, args->config_file, &config_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "Critical failure: parse_flipd_config() failed.");
        ret = EXIT_FAILURE;
        goto done;
    }

#ifndef DEBUG
    run_daemon(config_ctx->pid_file);
#endif

    ret = nbus_init_pair(mem_ctx, config_ctx->io_socket, &io_nbus_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "nbus_init_pair() failed.");
        ret = EXIT_FAILURE;
        goto done;
    }

    // -------------------------------------------------------------------------
    // Start crawler daemon
    struct worker_deamon_ctx *wd_ctx;

    wd_ctx = talloc_zero(mem_ctx, struct worker_deamon_ctx);
    if (wd_ctx == NULL) {
        LOG(LOG_ERR, "talloc_zero() failed.");
        ret = ENOMEM;
        goto done;
    }
    wd_ctx->identity_name = "alpha";
    wd_ctx->pid_file = "/tmp/flip_crawler_alpha.pid";
    wd_ctx->root_ipc = "ipc:///tmp/root_flip_crawler_alpha.ipc";

    ret = nbus_init_pair(mem_ctx, wd_ctx->root_ipc, &wd_ctx->nbus_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "nbus_init_pair() failed.");
        ret = EXIT_FAILURE;
        goto done;
    }

    struct string_ctx *hmm_chunk;
    ret = async_start_worker_daemon(mem_ctx, wd_ctx, &hmm_chunk);

    sleep(10);

    ret = EXIT_SUCCESS;
    goto done;
    // -------------------------------------------------------------------------

    int i = 30;
    while (i > 0) {

        ret = nbus_recieve(mem_ctx, wd_ctx->nbus_ctx, &chunk);
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

        LOG(LOG_CRIT, ">>> received: %s", chunk->data);

        i--;
        sleep(1);
    }

    ret = EXIT_SUCCESS;

done:

    stop_daemon();

    if (io_nbus_ctx != NULL) {
        ret = nbus_close(io_nbus_ctx);
        if (ret != EOK) {
            LOG(LOG_CRIT, "nbus_close() failed.");
            ret = EXIT_FAILURE;
        }
    }

    if (wd_ctx != NULL) {
        if (wd_ctx->nbus_ctx != NULL) {
            ret = nbus_close(wd_ctx->nbus_ctx);
            if (ret != EOK) {
                LOG(LOG_CRIT, "nbus_close() failed.");
                ret = EXIT_FAILURE;
            }
        }
        talloc_zfree(wd_ctx);
    }

    if (mem_ctx != NULL) {
        talloc_zfree(mem_ctx);
    }

    exit(ret);
}
