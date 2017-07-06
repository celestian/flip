#include <time.h>

#include "src/msg/msg.h"
#include "src/msg/msg_private.h"
#include "src/utils/errors.h"
#include "src/utils/logs.h"

errno_t create_message(TALLOC_CTX *mem_ctx, char *sender, char *recepient,
                       enum message_type message_type, void *data,
                       struct msg_ctx **_msg)
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
    talloc_free(tmp_ctx);
    return ret;
}