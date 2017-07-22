#ifndef __MSG_H__
#define __MSG_H__

#include <talloc.h>

#include "src/common/utils/data.h"
#include "src/common/utils/errors.h"

struct msg_ctx;

enum message_type {
    CRAWLER_ASK_FOR_CONFIGURATION,
    COLLECTOR_ASK_FOR_CONFIGURATION
};

errno_t create_message(TALLOC_CTX *mem_ctx, const char *sender,
                       const char *recepient, enum message_type message_type,
                       void *data, struct msg_ctx **_msg);

errno_t serialize_message(TALLOC_CTX *mem_ctx, struct msg_ctx *msg,
                          struct string_ctx **_serialized_msg);

#endif /* __MSG_H__ */