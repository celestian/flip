#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <talloc.h>

#include "src/utils/conf.h"
#include "src/utils/errors.h"
#include "src/utils/inih/ini.h"
#include "src/utils/logs.h"

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

static int collector_handler(void *data, const char *section, const char *name,
                             const char *value)
{
    struct config_ctx *config = (struct config_ctx *)data;

    if (MATCH("base", "work_dir")) {
        config->work_dir = talloc_strdup(config, value);
    } else if (MATCH("base", "pid_file")) {
        config->pid_file = talloc_strdup(config, value);
    } else if (MATCH("base", "io_socket")) {
        config->io_socket = talloc_strdup(config, value);
    } else {
        LOG(LOG_INFO, "Unknown configuration: %s.%s = '%s'", section, name,
            value);
    }

    return 1;
}

errno_t parse_flipd_config(TALLOC_CTX *mem_ctx, const char *filename,
                           struct config_ctx **_config_ctx)
{
    TALLOC_CTX *tmp_ctx;
    struct config_ctx *config_ctx = NULL;
    errno_t ret;

    tmp_ctx = talloc_new(NULL);
    if (tmp_ctx == NULL) {
        LOG(LOG_ERR, "talloc_new() failed.");
        return ENOMEM;
    }

    config_ctx = talloc_zero(tmp_ctx, struct config_ctx);
    if (config_ctx == NULL) {
        LOG(LOG_ERR, "talloc_zero() failed.");
        ret = ENOMEM;
        goto done;
    }

    ret = ini_parse(filename, collector_handler, (void *)config_ctx);
    if (ret != EOK) {
        LOG(LOG_ERR, "ini_parse() failed.");
        ret = EINVAL;
        goto done;
    }

    *_config_ctx = talloc_steal(mem_ctx, config_ctx);
    ret = EOK;

done:
    talloc_free(tmp_ctx);
    return ret;
}
