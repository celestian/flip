#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <nanomsg/bus.h>
#include <nanomsg/nn.h>
#include <nanomsg/pair.h>
#include <nanomsg/pubsub.h>
#include <talloc.h>

#include "nbus.h"
#include "nbus_private.h"
#include "src/common/utils/data.h"
#include "src/common/utils/logs.h"
#include "src/common/utils/utils.h"

static errno_t get_file_from_url(TALLOC_CTX *mem_ctx, const char *url,
                                 char **_file_name)
{
    TALLOC_CTX *tmp_ctx;
    const char *prefix_ipc = "ipc://";
    const char *remove = "";
    int remove_size;
    int url_size;
    char *file_name;
    int ret;

    tmp_ctx = talloc_new(NULL);
    if (tmp_ctx == NULL) {
        LOG(LOG_ERR, "talloc_new() failed.");
        return ENOMEM;
    }

    ret = strncmp(prefix_ipc, url, strlen(prefix_ipc));
    if (ret == 0) {
        remove = prefix_ipc;
    }

    remove_size = strlen(remove);
    url_size = strlen(url);

    file_name = talloc_array_size(tmp_ctx, sizeof(char *), url_size);
    if (file_name == NULL) {
        LOG(LOG_ERR, "talloc_array_size() failed.");
        ret = ENOMEM;
        goto done;
    }

    file_name
        = memcpy(file_name, url + remove_size, url_size - remove_size + 1);
    if (file_name == NULL) {
        LOG(LOG_ERR, "memcpy() failed.");
        ret = ENOMEM;
        goto done;
    }

    *_file_name = talloc_steal(mem_ctx, file_name);
    ret = EOK;

done:
    talloc_zfree(tmp_ctx);
    return ret;
}

static errno_t nbus_init_pubsub(TALLOC_CTX *mem_ctx, const char *url,
                                int protocol, struct nbus_ctx **_nbus_ctx)
{
    TALLOC_CTX *tmp_ctx;
    struct nbus_ctx *nbus_ctx = NULL;
    char *file_name;
    errno_t ret;

    tmp_ctx = talloc_new(NULL);
    if (tmp_ctx == NULL) {
        LOG(LOG_ERR, "talloc_new() failed.");
        return ENOMEM;
    }

    nbus_ctx = talloc_zero(tmp_ctx, struct nbus_ctx);
    if (nbus_ctx == NULL) {
        LOG(LOG_ERR, "talloc_zero() failed.");
        ret = ENOMEM;
        goto done;
    }

    nbus_ctx->sock_fd = nn_socket(AF_SP, protocol);
    if (nbus_ctx->sock_fd < 0) {
        LOG(LOG_ERR, "nn_socket() failed. [%d | %s]", errno,
            nn_strerror(errno));
        ret = ENOENT;
        goto done;
    }

    if (protocol == NN_SUB) {
        ret = nn_setsockopt(nbus_ctx->sock_fd, NN_SUB, NN_SUB_SUBSCRIBE, "", 0);
        if (ret < 0) {
            LOG(LOG_ERR, "nn_setsockopt() failed. [%d | %s]", errno,
                nn_strerror(errno));
            ret = ENOENT;
            goto done;
        }
    }

    ret = get_file_from_url(tmp_ctx, url, &file_name);
    if (ret != EOK) {
        LOG(LOG_ERR, "get_file_from_url() failed. [%d | %s]", ret,
            strerror(ret));
        goto done;
    }

    if (is_file_exist(file_name)) {
        nbus_ctx->endpoint_id = nn_connect(nbus_ctx->sock_fd, url);
        if (nbus_ctx->endpoint_id < 0) {
            LOG(LOG_ERR, "nn_connect() failed. [%d | %s]", errno,
                nn_strerror(errno));
            ret = ENOENT;
            goto done;
        }
    } else {
        nbus_ctx->endpoint_id = nn_bind(nbus_ctx->sock_fd, url);
        if (nbus_ctx->endpoint_id < 0) {
            LOG(LOG_ERR, "nn_bind() failed. [%d | %s]", errno,
                nn_strerror(errno));
            ret = ENOENT;
            goto done;
        }
    }

    *_nbus_ctx = talloc_steal(mem_ctx, nbus_ctx);
    ret = EOK;

done:
    talloc_zfree(tmp_ctx);
    return ret;
}

errno_t nbus_init_pair(TALLOC_CTX *mem_ctx, const char *url,
                       struct nbus_ctx **_nbus_ctx)
{
    return nbus_init_pubsub(mem_ctx, url, NN_BUS, _nbus_ctx);
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
    int ret;

    if (nbus_ctx == NULL) {
        ret = EOK;
        goto done;
    }

    ret = nn_shutdown(nbus_ctx->sock_fd, nbus_ctx->endpoint_id);
    LOG(LOG_CRIT, "nn_shutdown: %d", ret);
    if (ret < 0) {
        LOG(LOG_ERR, "nn_shutdown() failed. [%d | %s]", errno,
            nn_strerror(errno));
        ret = ENOENT;
        goto done;
    }

    talloc_zfree(nbus_ctx);
    ret = EOK;

done:
    return ret;
}

errno_t nbus_send(struct nbus_ctx *nbus_ctx, const char *msg, size_t msg_size)
{
    int bytes;
    int ret;

    if (nbus_ctx == NULL) {
        LOG(LOG_ERR, "nbus_ctx is NULL.");
        ret = ENOENT;
        goto done;
    }

    bytes = nn_send(nbus_ctx->sock_fd, msg, msg_size, 0);
    if (bytes < 0 || bytes != msg_size) {
        LOG(LOG_ERR, "nn_send() failed. [%d | %s]", errno, nn_strerror(errno));
        ret = EIO;
        goto done;
    }

    ret = EOK;

done:
    return ret;
}

errno_t nbus_recieve(TALLOC_CTX *mem_ctx, struct nbus_ctx *nbus_ctx,
                     struct string_ctx **_chunk)
{
    TALLOC_CTX *tmp_ctx;
    void *buffer = NULL;
    int msg_size;
    struct string_ctx *chunk;
    errno_t ret;

    if (nbus_ctx == NULL) {
        LOG(LOG_ERR, "nbus_ctx is NULL.");
        return ENOENT;
    }

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

    msg_size = nn_recv(nbus_ctx->sock_fd, &buffer, NN_MSG, NN_DONTWAIT);
    if (msg_size < 0 && errno != EAGAIN) {
        LOG(LOG_ERR, "nn_recv() failed. [%d | %s]", errno, nn_strerror(errno));
        ret = EIO;
        goto done;
    }
    if (msg_size < 0 && errno == EAGAIN) {
        /* There's no data -- it's not a fail. */
        ret = EAGAIN;
        goto done;
    }

    chunk->data = talloc_strndup(chunk->data, buffer, msg_size);
    if (chunk->data == NULL) {
        LOG(LOG_ERR, "talloc_strndup() failed.");
        ret = ENOMEM;
        goto done;
    }
    chunk->size = talloc_array_length(chunk->data);
    nn_freemsg(buffer);

    *_chunk = talloc_steal(mem_ctx, chunk);
    ret = EOK;

done:
    talloc_zfree(tmp_ctx);
    return ret;
}

int nbus_get_sock_fd(struct nbus_ctx *nbus_ctx) { return nbus_ctx->sock_fd; }