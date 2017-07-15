#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <talloc.h>

#include "src/module/flipd/daemon_handler.h"
#include "src/msg/msg.h"
#include "src/nbus/nbus.h"
#include "src/url/url.h"
#include "src/utils/args.h"
#include "src/utils/conf.h"
#include "src/utils/daemon.h"
#include "src/utils/data.h"
#include "src/utils/errors.h"
#include "src/utils/logs.h"

int main(int argc, char *argv[])
{
    TALLOC_CTX *mem_ctx = NULL;
    struct flipd_args_ctx *args;
    struct config_ctx *config_ctx;
    struct nbus_ctx *io_nbus_ctx = NULL;
    struct nbus_ctx *wd_nbus_ctx = NULL;
    struct string_ctx *chunk;
    errno_t ret;

    log_init(FLIPD);

    mem_ctx = talloc_new(NULL);
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

    // Start crawler daemon
    char *wd_identity_name = "alpha";
    char *wd_pid_file = "/tmp/flip_crawler_alpha.pid";
    char *wd_root_ipc = "ipc:///tmp/root_flip_crawler_alpha.ipc";

    ret = nbus_init_pair(mem_ctx, wd_root_ipc, &wd_nbus_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "nbus_init_pair() failed.");
        ret = EXIT_FAILURE;
        goto done;
    }

    char *command = talloc_asprintf(
        mem_ctx, "/home/celestian/Projects/flip/x86_64/flip_crawler %s %s %s",
        wd_identity_name, wd_pid_file, wd_root_ipc);

    ret = system(command);
    if (ret != EOK) {
        LOG(LOG_CRIT, "system() failed.");
        ret = EXIT_FAILURE;
        goto done;
    }
    talloc_free(command);

    // Waiting for ask for configuration ---------------------------------------

    ret = get_answer(mem_ctx, wd_nbus_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "get_answer() failed.");
        ret = EXIT_FAILURE;
        goto done;
    }

    ret = EXIT_SUCCESS;
    goto done;
    // -------------------------------------------------------------------------

    int i = 30;
    while (i > 0) {

        ret = nbus_recieve(mem_ctx, wd_nbus_ctx, &chunk);
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

    if (wd_nbus_ctx != NULL) {
        ret = nbus_close(wd_nbus_ctx);
        if (ret != EOK) {
            LOG(LOG_CRIT, "nbus_close() failed.");
            ret = EXIT_FAILURE;
        }
        talloc_zfree(wd_ctx);
    }

    if (mem_ctx != NULL) {
        talloc_zfree(mem_ctx);
    }

    exit(ret);
}
