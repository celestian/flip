#include <talloc.h>

#include "src/common/msg/msg.h"
#include "src/common/nbus/nbus.h"
#include "src/common/utils/logs.h"
#include "src/common/utils/utils.h"
#include "src/module/common_daemon.h"

errno_t get_config_from_root_daemon(TALLOC_CTX *mem_ctx,
                                    struct nbus_ctx *root_nbus_ctx,
                                    const char *identity_name,
                                    enum daemon_type daemon_type,
                                    char **_config_json)
{
    TALLOC_CTX *tmp_ctx;
    enum message_type message_type;
    struct msg_ctx *msg = NULL;
    struct string_ctx *serialized_msg = NULL;
    errno_t ret;

    tmp_ctx = talloc_new(NULL);
    if (tmp_ctx == NULL) {
        LOG(LOG_ERR, "talloc_new() failed.");
        return ENOMEM;
    }

    switch (daemon_type) {
    case CRAWLER:
        message_type = CRAWLER_ASK_FOR_CONFIGURATION;
        break;
    case COLLECTOR:
        message_type = COLLECTOR_ASK_FOR_CONFIGURATION;
        break;
    default:
        LOG(LOG_CRIT, "Wrong daemon type: cannot choose message type.");
        ret = EINVAL;
        goto done;
        break;
    }

    ret = create_message(tmp_ctx, identity_name, "root", message_type, NULL,
                         &msg);
    if (ret != EOK) {
        LOG(LOG_CRIT, "create_message() failed.");
        ret = EINVAL;
        goto done;
    }

    ret = serialize_message(tmp_ctx, msg, &serialized_msg);
    if (ret != EOK) {
        LOG(LOG_CRIT, "serialize_message() failed.");
        ret = EINVAL;
        goto done;
    }

    LOG(LOG_CRIT, "about asking");
    ret = nbus_send(root_nbus_ctx, serialized_msg->data, serialized_msg->size);
    LOG(LOG_CRIT, "after asking");
    if (ret != EOK) {
        LOG(LOG_CRIT, "nbus_send() failed.");
        ret = EINVAL;
        goto done;
    }

    ret = EOK;

done:
    talloc_zfree(tmp_ctx);
    return ret;
}
