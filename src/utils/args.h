#ifndef __ARGS_H__
#define __ARGS_H__

#include <talloc.h>

#include "src/utils/errors.h"

enum worker_type { CRAWLER, COLLECTOR };

struct worker_args_ctx {
    char *identity_name;
    char *pid_file;
    char *root_ipc;
};

errno_t parse_worker_args(TALLOC_CTX *mem_ctx, int argc, char *argv[],
                          enum worker_type wtype,
                          struct worker_args_ctx **_args);

#endif /* __ARGS_H__ */