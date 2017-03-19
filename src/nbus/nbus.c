#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <nanomsg/nn.h>
#include <nanomsg/pubsub.h>
#include <talloc.h>

#include "nbus.h"
#include "nbus_private.h"
#include "src/utils/errors.h"

errno_t nbus_init(TALLOC_CTX* mem_ctx, const char* url,
                  struct nbus_ctx** _nbus_ctx)
{
    TALLOC_CTX* tmp_ctx;
    struct nbus_ctx* nbus_ctx = NULL;
    errno_t ret;

    tmp_ctx = talloc_new(NULL);
    if (tmp_ctx == NULL) {
        return ENOMEM;
    }

    nbus_ctx = talloc_zero(tmp_ctx, struct nbus_ctx);
    if (nbus_ctx == NULL) {
        ret = ENOMEM;
        goto done;
    }

    nbus_ctx->sock_fd = nn_socket(AF_SP, NN_PUB);
    if (nbus_ctx->sock_fd < 0) {
        printf("nn_socket() failed [%d]: %s\n", nn_errno(), nn_strerror(errno));
        ret = ENOENT;
        goto done;
    }

    nbus_ctx->endpoint_id = nn_bind(nbus_ctx->sock_fd, url);
    if (nbus_ctx->endpoint_id <= 0) {
        printf("nn_bind() failed [%d]: %s\n", nn_errno(), nn_strerror(errno));
        ret = ENOENT;
        goto done;
    }

    *_nbus_ctx = talloc_steal(mem_ctx, nbus_ctx);
    ret = EOK;

done:
    talloc_free(tmp_ctx);
    return ret;
}

errno_t nbus_close(struct nbus_ctx* nbus_ctx)
{

    if (nbus_ctx == NULL) {
        return EOK;
    }

    nn_shutdown(nbus_ctx->sock_fd, nbus_ctx->endpoint_id);
    talloc_free(nbus_ctx);

    return EOK;
}

errno_t nbus_send(struct nbus_ctx* nbus_ctx, const char* msg, size_t msg_size)
{

    int bytes;

    if (nbus_ctx == NULL) {
        printf("shit\n");
        return ENOENT;
    }

    bytes = nn_send(nbus_ctx->sock_fd, msg, msg_size, 0);
    if (bytes < 0) {
        printf("nn_send() failed [%d]: %s\n", nn_errno(), nn_strerror(errno));
    }
    printf(">>> %d\n", bytes);
    // assert(bytes == msg_size);

    return EOK;
}
