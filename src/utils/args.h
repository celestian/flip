#ifndef __ARGS_H__
#define __ARGS_H__

#include <talloc.h>

#include "src/utils/errors.h"
#include "src/utils/utils.h"

struct worker_args_ctx {
    char *identity_name;
    char *pid_file;
    char *root_ipc;
};

errno_t parse_worker_args(TALLOC_CTX *mem_ctx, int argc, char *argv[],
                          enum daemon_type daemon_type,
                          struct worker_args_ctx **_args);

#endif /* __ARGS_H__ */