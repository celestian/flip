#include <talloc.h>
#include <tevent.h>

#include "src/module/flipd/daemon_handler.h"
#include "src/nbus/nbus.h"
#include "src/utils/errors.h"
#include "src/utils/logs.h"

/*
struct write_pipe_state {
    int fd;
    uint8_t *buf;
    size_t len;
    ssize_t written;
};

static void write_pipe_handler(struct tevent_context *ev,
                               struct tevent_fd *fde,
                               uint16_t flags, void *pvt);

struct tevent_req *write_pipe_send(TALLOC_CTX *mem_ctx,
                                   struct tevent_context *ev,
                                   uint8_t *buf, size_t len, int fd)
{
    struct tevent_req *req;
    struct write_pipe_state *state;
    struct tevent_fd *fde;

    req = tevent_req_create(mem_ctx, &state, struct write_pipe_state);
    if (req == NULL) return NULL;

    state->fd = fd;
    state->buf = buf;
    state->len = len;
    state->written = 0;

    fde = tevent_add_fd(ev, state, fd, TEVENT_FD_WRITE,
                        write_pipe_handler, req);
    if (fde == NULL) {
        DEBUG(SSSDBG_CRIT_FAILURE, "tevent_add_fd failed.\n");
        goto fail;
    }

    return req;

fail:
    talloc_zfree(req);
    return NULL;
}

static void write_pipe_handler(struct tevent_context *ev,
                               struct tevent_fd *fde,
                               uint16_t flags, void *pvt)
{
    struct tevent_req *req = talloc_get_type(pvt, struct tevent_req);
    struct write_pipe_state *state = tevent_req_data(req,
                                                     struct write_pipe_state);
    errno_t ret;

    if (flags & TEVENT_FD_READ) {
        DEBUG(SSSDBG_CRIT_FAILURE,
              "write_pipe_done called with TEVENT_FD_READ,"
               " this should not happen.\n");
        tevent_req_error(req, EINVAL);
        return;
    }

    errno = 0;
    state->written = sss_atomic_write_s(state->fd, state->buf, state->len);
    if (state->written == -1) {
        ret = errno;
        DEBUG(SSSDBG_CRIT_FAILURE,
              "write failed [%d][%s].\n", ret, strerror(ret));
        tevent_req_error(req, ret);
        return;
    }

    if (state->len != state->written) {
        DEBUG(SSSDBG_CRIT_FAILURE, "Wrote %zd bytes, expected %zu\n",
              state->written, state->len);
        tevent_req_error(req, EIO);
        return;
    }

    DEBUG(SSSDBG_TRACE_FUNC, "All data has been sent!\n");
    tevent_req_done(req);
    return;
}

int write_pipe_recv(struct tevent_req *req)
{
    TEVENT_REQ_RETURN_ON_ERROR(req);

    return EOK;
}
*/

struct communication_ctx {
    TALLOC_CTX *mem_ctx;
    struct nbus_ctx *nbus_ctx;
};

static void handler(struct tevent_context *ev, struct tevent_fd *fde,
                    uint16_t flags, void *private_data)
{
    struct string_ctx *chunk;
    errno_t ret;

    struct communication_ctx *cctx
        = talloc_get_type(private_data, struct communication_ctx);

    // handling event; reading from a file descriptor
    ret = nbus_recieve(cctx->mem_ctx, cctx->nbus_ctx, &chunk);
    if (ret != EOK) {
        LOG(LOG_CRIT, "Critical failure: nbus_recieve() failed.");
        talloc_free(cctx);
        exit(EXIT_FAILURE);
    }

    LOG(LOG_CRIT, ">>> received: %s", chunk->data);
}

errno_t get_answer(TALLOC_CTX *mem_ctx, struct nbus_ctx *nbus_ctx)
{
    TALLOC_CTX *tmp_ctx;
    struct tevent_context *event_ctx = NULL;
    struct tevent_fd *fd_event = NULL;
    struct communication_ctx *c_ctx = NULL;
    errno_t ret;

    tmp_ctx = talloc_new(NULL);
    if (tmp_ctx == NULL) {
        LOG(LOG_ERR, "talloc_new() failed.");
        return ENOMEM;
    }

    c_ctx = talloc_zero(tmp_ctx, struct communication_ctx);
    if (nbus_ctx == NULL) {
        LOG(LOG_ERR, "talloc_zero() failed.");
        ret = ENOMEM;
        goto done;
    }

    c_ctx->mem_ctx = tmp_ctx;
    c_ctx->nbus_ctx = nbus_ctx;

    event_ctx = tevent_context_init(tmp_ctx);
    if (event_ctx == NULL) {
        LOG(LOG_CRIT, "tevent_context_init() failed.");
        ret = ENOMEM;
        goto done;
    }

    fd_event = tevent_add_fd(event_ctx, tmp_ctx, nbus_get_sock_fd(nbus_ctx),
                             TEVENT_FD_READ, handler, c_ctx);
    if (fd_event == NULL) {
        LOG(LOG_CRIT, "tevent_add_fd() failed.");
        ret = EXIT_FAILURE;
        goto done;
    }

    ret = tevent_loop_once(fd_event);

    ret = EOK;

done:
    talloc_free(tmp_ctx);
    return ret;
}