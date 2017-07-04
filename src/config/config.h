#ifndef __CONF_H__
#define __CONF_H__

#include <stddef.h>
#include <talloc.h>

#include "src/utils/errors.h"

struct config_ctx {
    char *work_dir;
    char *socket;
    char *db;
};

errno_t parse_config(TALLOC_CTX *mem_ctx, const char *filename,
                     struct config_ctx **_config_ctx);

#endif /* __CONF_H__ */