#ifndef __MSG_H__
#define __MSG_H__

#include <talloc.h>

#include "src/utils/errors.h"

struct msg_ctx;

enum message_type { ASK_FOR_CONFIGURATION };

errno_t create_message(TALLOC_CTX *mem_ctx, char *sender, char *recepient,
                       enum message_type message_type, void *data,
                       struct msg_ctx **_msg);

#endif /* __MSG_H__ */