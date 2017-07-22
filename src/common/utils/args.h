#ifndef __ARGS_H__
#define __ARGS_H__

#include <talloc.h>

#include "src/common/utils/errors.h"
#include "src/common/utils/utils.h"

struct flipd_args_ctx {
    char *config_file;
};

struct worker_args_ctx {
    char *identity_name;
    char *pid_file;
    char *root_ipc;
};

errno_t parse_flipd_args(TALLOC_CTX *mem_ctx, int argc, char *argv[],
                         struct flipd_args_ctx **_args);

errno_t parse_worker_args(TALLOC_CTX *mem_ctx, int argc, char *argv[],
                          enum daemon_type daemon_type,
                          struct worker_args_ctx **_args);

#endif /* __ARGS_H__ */