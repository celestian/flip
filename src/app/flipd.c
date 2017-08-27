#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <nanomsg/nn.h>
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

    LOG(LOG_CRIT, "START");

    struct nn_pollfd pfd[1];
    pfd[0].fd = nbus_get_sock_fd(io_nbus_ctx);
    pfd[0].events = NN_POLLIN;

    while (true) {

        ret = nn_poll(pfd, 1, 2000);
        if (ret < 0) {
            LOG(LOG_CRIT, "nn_poll() failed.");
            ret = EXIT_SUCCESS;
            goto done;
        }
        if (ret > 0) {
            ret = nbus_recieve(main_ctx, io_nbus_ctx, &chunk);
            if (ret != EOK && ret != EAGAIN) {
                LOG(LOG_CRIT, "Critical failure: nbus_recieve() failed.");
                talloc_zfree(main_ctx);
                exit(EXIT_FAILURE);
            }
            if (ret == EAGAIN) {
                continue;
            }

            LOG(LOG_CRIT, ">>> received: %s %d", chunk->data, NN_POLLIN);
            if (strncmp(chunk->data, "end", 3) == 0) {
                ret = EXIT_SUCCESS;
                goto done;
            }
        }
    }

    sleep(3);
    ret = EXIT_SUCCESS;
    goto done;

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

/*
char *command = talloc_asprintf(
    tmp_ctx, "/home/pcech/Projects/flip/x86_64/flip_crawler %s %s %s ",
    identity_name, pid_file, url_root_ipc);

ret = system(command);
if (ret != EOK) {
    LOG(LOG_CRIT, "system() failed.");
    ret = EIO;
    goto done;
}
talloc_zfree(command);
*/

// -------------------------------------------------------------------------

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
