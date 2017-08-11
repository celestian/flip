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
#include "src/module/flipd/listen_handler.h"

int main(int argc, char *argv[])
{
    struct main_context *main_ctx = NULL;
    struct flipd_args_ctx *args;
    struct config_ctx *config_ctx;
    struct nbus_ctx *io_nbus_ctx = NULL;
    struct string_ctx *chunk;
    errno_t ret;

    ret = init_main_context(FLIPD, &main_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "Critical failure: init_main_context() failed.");
        ret = EXIT_FAILURE;
        goto done;
    }

    ret = parse_flipd_args(main_ctx, argc, argv, &args);
    if (ret != EOK) {
        LOG(LOG_CRIT, "Critical failure: parse_flipd_args() failed.");
        ret = EXIT_FAILURE;
        goto done;
    }

    ret = parse_flipd_config(main_ctx, args->config_file, &config_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "Critical failure: parse_flipd_config() failed.");
        ret = EXIT_FAILURE;
        goto done;
    }

#ifndef DEBUG
    become_daemon(config_ctx->pid_file);
#endif

    ret = nbus_init_pair(main_ctx, config_ctx->io_socket, &io_nbus_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "nbus_init_pair() failed.");
        ret = EXIT_FAILURE;
        goto done;
    }

    ret = async_listen(main_ctx, main_ctx->event_ctx, io_nbus_ctx);
    LOG(LOG_CRIT, "async_listen: %d", ret);

    LOG(LOG_CRIT, ">>> start");

    tevent_loop_wait(main_ctx->event_ctx);
    // talloc_free(main_ctx->event_ctx);

    LOG(LOG_CRIT, ">>> end");

    sleep(3);

    ret = EXIT_SUCCESS;
    goto done;

    // -------------------------------------------------------------------------

    ret = async_start_worker_daemon(main_ctx, "alpha",
                                    "/tmp/flip_crawler_alpha.pid",
                                    "ipc:///tmp/root_flip_crawler_alpha.ipc");

    LOG(LOG_CRIT, "async_start_worker_daemon() = %i", ret);

    sleep(10);

    ret = EXIT_SUCCESS;
    goto done;
    // -------------------------------------------------------------------------
    // Start crawler daemon
    struct worker_daemon_ctx *wd_ctx;

    wd_ctx = talloc_zero(main_ctx, struct worker_daemon_ctx);
    if (wd_ctx == NULL) {
        LOG(LOG_ERR, "talloc_zero() failed.");
        ret = ENOMEM;
        goto done;
    }
    wd_ctx->identity_name = "alpha";
    wd_ctx->pid_file = "/tmp/flip_crawler_alpha.pid";
    wd_ctx->url_root_ipc = "ipc:///tmp/root_flip_crawler_alpha.ipc";

    ret = nbus_init_pair(main_ctx, wd_ctx->url_root_ipc, &wd_ctx->nbus_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "nbus_init_pair() failed.");
        ret = EXIT_FAILURE;
        goto done;
    }

    struct string_ctx *hmm_chunk;
    ret = async_start_worker_daemon2(main_ctx, wd_ctx, &hmm_chunk);

    sleep(10);

    ret = EXIT_SUCCESS;
    goto done;
    // -------------------------------------------------------------------------

    int i = 30;
    while (i > 0) {

        ret = nbus_recieve(main_ctx, wd_ctx->nbus_ctx, &chunk);
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

    /*
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
    */

    if (main_ctx != NULL) {
        talloc_zfree(main_ctx);
    }

    exit(ret);
}
