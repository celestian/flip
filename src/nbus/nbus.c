#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <nanomsg/nn.h>
#include <nanomsg/pubsub.h>
#include <talloc.h>

#include "nbus.h"
#include "nbus_private.h"
#include "src/utils/data.h"
#include "src/utils/errors.h"
#include "src/utils/logs.h"

static errno_t nbus_init_pubsub(TALLOC_CTX *mem_ctx, const char *url,
                                int protocol, struct nbus_ctx **_nbus_ctx)
{
    TALLOC_CTX *tmp_ctx;
    struct nbus_ctx *nbus_ctx = NULL;
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

    nbus_ctx->sock_fd = nn_socket(AF_SP, protocol);
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

errno_t nbus_init_pub(TALLOC_CTX *mem_ctx, const char *url,
                      struct nbus_ctx **_nbus_ctx)
{
    return nbus_init_pubsub(mem_ctx, url, NN_PUB, _nbus_ctx);
}

errno_t nbus_init_sub(TALLOC_CTX *mem_ctx, const char *url,
                      struct nbus_ctx **_nbus_ctx)
{
    return nbus_init_pubsub(mem_ctx, url, NN_SUB, _nbus_ctx);
}

errno_t nbus_close(struct nbus_ctx *nbus_ctx)
{

    if (nbus_ctx == NULL) {
        return EOK;
    }

    nn_shutdown(nbus_ctx->sock_fd, nbus_ctx->endpoint_id);
    // TODO: Errors!!
    talloc_free(nbus_ctx);

    return EOK;
}

errno_t nbus_send(struct nbus_ctx *nbus_ctx, const char *msg, size_t msg_size)
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
    // assert(bytes == msg_size);

    LOG(LOG_NOTICE, "Test B.");

    return EOK;
}

errno_t nbus_recieve(TALLOC_CTX *mem_ctx, struct nbus_ctx *nbus_ctx,
                     struct string_ctx **_chunk)
{
    TALLOC_CTX *tmp_ctx;
    void *buffer = NULL;
    int msg_size;
    struct string_ctx *chunk;
    errno_t ret;

    tmp_ctx = talloc_new(NULL);
    if (tmp_ctx == NULL) {
        return ENOMEM;
    }

    chunk = talloc_zero(tmp_ctx, struct string_ctx);
    if (chunk == NULL) {
        ret = ENOMEM;
        LOG(LOG_ERR, "talloc_zero() failed. [%d | %s]", ret, strerror(ret));
        goto done;
    }

    msg_size = nn_recv(nbus_ctx->sock_fd, &buffer, NN_MSG, 0);
    if (msg_size < 0) {
        // TODO: EAGAIN has special meaning.
        LOG(LOG_ERR, "nn_recv() failed. [%d | %s]", errno, nn_strerror(errno));
        ret = errno;
        goto done;
    }

    chunk->data = talloc_strndup(chunk->data, buffer, msg_size);
    if (chunk->data == NULL) {
        ret = ENOMEM;
        LOG(LOG_ERR, "talloc_strndup() failed. [%d | %s]", ret, strerror(ret));
        goto done;
    }
    chunk->size = talloc_array_length(chunk->data);
    nn_freemsg(buffer);

    *_chunk = talloc_steal(mem_ctx, chunk);
    ret = EOK;

done:
    talloc_free(tmp_ctx);
    return ret;
}
