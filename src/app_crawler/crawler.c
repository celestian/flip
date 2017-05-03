#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "src/log/logs.h"
#include "src/nbus/nbus.h"
#include "src/url/url.h"
#include "src/utils/data.h"
#include "src/utils/errors.h"

int main_module(void)
{
    TALLOC_CTX* mem_ctx;
    struct url_conn_ctx* url_conn_ctx;
    struct nbus_ctx* nbus_ctx;
    struct string_ctx* chunk;
    errno_t ret;

    log_init("flip_listener");

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

int main(int argc, char* argv[])
{
    pid_t pid = 0;
    pid_t sid = 0;
    int ret;
    bool next_iteration = true;

    pid = fork();
    if (pid < 0) {
        printf("[error] Daemon didn't start [fork()].\n");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    umask(0);

    sid = setsid();
    if (sid < 0) {
        printf("[error] Daemon didn't start [setsid()].\n");
        exit(EXIT_FAILURE);
    }

    ret = chdir("/tmp");
    if (ret < 0) {
        printf("[error] Daemon didn't start [chdir() [%i] [%s]].\n", ret,
               strerror(ret));
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    while (next_iteration) {
        main_module();
        next_iteration = false;
    }

    exit(EXIT_SUCCESS);
}
