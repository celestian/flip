#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <talloc.h>

#include "src/conf/conf.h"
#include "src/conf/inih/ini.h"
#include "src/utils/errors.h"
#include "src/utils/logs.h"

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

static int collector_handler(void *data, const char *section, const char *name,
                             const char *value)
{
    struct collector_conf_ctx *config = (struct collector_conf_ctx *)data;

    if (MATCH("base", "socket")) {
        config->socket = talloc_strdup(config, value);
    } else if (MATCH("base", "db")) {
        config->db = talloc_strdup(config, value);
    } else {
        return 0; /* unknown section/name, error */
    }

    return 1;
}

errno_t parse_collector_conf(TALLOC_CTX *mem_ctx, const char *filename,
                             struct collector_conf_ctx **_conf_ctx)
{
    TALLOC_CTX *tmp_ctx;
    struct collector_conf_ctx *conf_ctx = NULL;
    errno_t ret;

    tmp_ctx = talloc_new(NULL);
    if (tmp_ctx == NULL) {
        LOG(LOG_ERR, "talloc_new() failed.");
        return ENOMEM;
    }

    conf_ctx = talloc_zero(tmp_ctx, struct collector_conf_ctx);
    if (conf_ctx == NULL) {
        LOG(LOG_ERR, "talloc_zero() failed.");
        ret = ENOMEM;
        goto done;
    }

    ret = ini_parse(filename, collector_handler, (void *)conf_ctx);
    if (ret != EOK) {
        LOG(LOG_ERR, "ini_parse() failed.");
        ret = EINVAL;
        goto done;
    }

    *_conf_ctx = talloc_steal(mem_ctx, conf_ctx);
    ret = EOK;

done:
    talloc_free(tmp_ctx);
    return ret;
}