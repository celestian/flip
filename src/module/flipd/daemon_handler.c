#include <talloc.h>
#include <tevent.h>
#include <unistd.h>

#include "src/module/flipd/daemon_handler.h"
#include "src/common/nbus/nbus.h"
#include "src/common/utils/data.h"
#include "src/common/utils/errors.h"
#include "src/common/utils/logs.h"

errno_t async_start_worker_daemon2(TALLOC_CTX *mem_ctx,
                                   struct worker_daemon_ctx *wd_ctx,
                                   struct string_ctx **_chunk)
{
    TALLOC_CTX *tmp_ctx;
    struct string_ctx *chunk;
    errno_t ret;

    tmp_ctx = talloc_new(NULL);
    if (tmp_ctx == NULL) {
        LOG(LOG_ERR, "talloc_new() failed.");
        return ENOMEM;
    }

    chunk = talloc_zero(tmp_ctx, struct string_ctx);
    if (chunk == NULL) {
        LOG(LOG_ERR, "talloc_zero() failed.");
        ret = ENOMEM;
        goto done;
    }

    for (int i = 30; i > 0; i--) {
        if (i == 30) {
            char *command = talloc_asprintf(
                tmp_ctx,
                "/home/celestian/Projects/flip/x86_64/flip_crawler %s %s %s ",
                wd_ctx->identity_name, wd_ctx->pid_file, wd_ctx->root_ipc);

            ret = system(command);
            if (ret != EOK) {
                LOG(LOG_CRIT, "system() failed.");
                ret = EIO;
                goto done;
            }
            talloc_zfree(command);
        }

        ret = nbus_recieve(mem_ctx, wd_ctx->nbus_ctx, &chunk);
        if (ret != EOK && ret != EAGAIN) {
            LOG(LOG_CRIT, "Critical failure: nbus_recieve() failed.");
            ret = EIO;
            goto done;
        }
        if (ret == EAGAIN) {
            sleep(1);
            continue;
        }

        LOG(LOG_CRIT, "Received: %s", chunk->data);
    }

    *_chunk = talloc_steal(mem_ctx, chunk);
    ret = EOK;

done:
    if (tmp_ctx != NULL) {
        talloc_zfree(tmp_ctx);
    }

    return ret;
}
