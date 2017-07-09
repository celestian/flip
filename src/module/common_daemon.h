#ifndef __COMMON_DAEMON__
#define __COMMON_DAEMON__

#include <talloc.h>

#include "src/nbus/nbus.h"
#include "src/utils/errors.h"
#include "src/utils/utils.h"

errno_t get_config_from_root_daemon(TALLOC_CTX *mem_ctx,
                                    struct nbus_ctx *root_nbus_ctx,
                                    const char *identity_name,
                                    enum daemon_type daemon_type,
                                    char **_config_json);

#endif /* __COMMON_DAEMON__ */