#ifndef __CONF_H__
#define __CONF_H__

#include <stddef.h>
#include <talloc.h>

#include "src/utils/errors.h"

struct collector_conf_ctx {
    char *socket;
    char *db;
};

errno_t parse_collector_conf(TALLOC_CTX *mem_ctx, const char *filename,
                             struct collector_conf_ctx **_conf_ctx);

#endif /* __CONF_H__ */