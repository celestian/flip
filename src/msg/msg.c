#include <string.h>
#include <time.h>

#include "src/msg/msg.h"
#include "src/msg/msg_private.h"
#include "src/utils/data.h"
#include "src/utils/errors.h"
#include "src/utils/logs.h"

errno_t create_message(TALLOC_CTX *mem_ctx, const char *sender,
                       const char *recepient, enum message_type message_type,
                       void *data, struct msg_ctx **_msg)
{
    TALLOC_CTX *tmp_ctx;
    struct msg_ctx *msg;
    errno_t ret;

    tmp_ctx = talloc_new(NULL);
    if (tmp_ctx == NULL) {
        LOG(LOG_ERR, "talloc_new() failed.");
        return ENOMEM;
    }

    msg = talloc_zero(tmp_ctx, struct msg_ctx);
    if (msg == NULL) {
        LOG(LOG_ERR, "talloc_zero() failed.");
        ret = ENOMEM;
        goto done;
    }

    msg->sender = talloc_strdup(msg, sender);
    msg->recepient = talloc_strdup(msg, recepient);
    msg->timestamp = time(NULL);
    msg->data = talloc_steal(msg, data);

    *_msg = talloc_steal(mem_ctx, msg);
    ret = EOK;

done:
    talloc_zfree(tmp_ctx);
    return ret;
}

errno_t serialize_message(TALLOC_CTX *mem_ctx, struct msg_ctx *msg,
                          struct string_ctx **_serialized_msg)
{
    TALLOC_CTX *tmp_ctx;
    struct string_ctx *serialized_msg;
    errno_t ret;

    const char *msg_schema = "{\"sender\": \"%s\", \"recepient\":\"%s\","
                             " \"timestamp\": %lu, \"message_type\": %d,"
                             " \"message_data\": %s}";

    tmp_ctx = talloc_new(NULL);
    if (tmp_ctx == NULL) {
        LOG(LOG_ERR, "talloc_new() failed.");
        return ENOMEM;
    }

    serialized_msg = talloc_zero(tmp_ctx, struct string_ctx);
    if (msg == NULL) {
        LOG(LOG_ERR, "talloc_zero() failed.");
        ret = ENOMEM;
        goto done;
    }

    serialized_msg->data = talloc_asprintf(
        serialized_msg, msg_schema, msg->sender, msg->recepient,
        (unsigned long)msg->timestamp, msg->message_type,
        (msg->data == NULL) ? "{}" : msg->data);
    serialized_msg->size = strlen(serialized_msg->data);

    *_serialized_msg = talloc_steal(mem_ctx, serialized_msg);
    ret = EOK;

done:
    talloc_zfree(tmp_ctx);
    return ret;
}
