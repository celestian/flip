#include <stdio.h>
#include <stdlib.h>

#include "src/common/nbus/nbus.h"
#include "src/common/utils/daemon.h"
#include "src/common/utils/logs.h"

int main(int argc, char *argv[])
{
    struct main_context *main_ctx = NULL;
    struct nbus_ctx *io_nbus_ctx = NULL;
    errno_t ret;

    ret = init_main_context(FLIP, &main_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "Critical failure: init_main_context() failed.");
        ret = EXIT_FAILURE;
        goto done;
    }

    printf(">>> Connecting...\n");

    LOG(LOG_CRIT, ">>> start");

    ret = is_file_exist("ipc:///tmp/flipd.ipc");
    printf("File is exist? %d", ret);

    ret = nbus_init_pair(main_ctx, "ipc:///tmp/flipd.ipc", &io_nbus_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "nbus_init_pair() failed.");
        ret = EXIT_FAILURE;
        goto done;
    }

    printf(">>> Sending...\n");
    ret = nbus_send(io_nbus_ctx, "hello", 4);
    if (ret != EOK) {
        LOG(LOG_CRIT, "Critical failure: Not enough memory.");
        exit(EXIT_FAILURE);
    }

    printf(">>> Sended!\n");

    ret = nbus_close(io_nbus_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "nbus_close() failed.");
        ret = EXIT_FAILURE;
        goto done;
    }

    LOG(LOG_CRIT, ">>> end");

    ret = EXIT_SUCCESS;
    goto done;

done:
    exit(ret);
}
