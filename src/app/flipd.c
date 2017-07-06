#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <talloc.h>

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
    TALLOC_CTX *mem_ctx;
    struct config_ctx *config_ctx;
    errno_t ret;

    log_init(CRAWLER);

    mem_ctx = talloc_new(NULL);
    if (mem_ctx == NULL) {
        LOG(LOG_CRIT, "Critical failure: Not enough memory.");
        exit(EXIT_FAILURE);
    }

    ret = parse_flipd_config(mem_ctx, "This must be read from command line",
                             &config_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "Critical failure: parse_flipd_config() failed.");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
