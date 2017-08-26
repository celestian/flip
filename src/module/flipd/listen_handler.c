#include "src/module/flipd/listen_handler.h"

#include "src/common/nbus/nbus.h"
#include "src/common/utils/logs.h"

struct async_listen_state {
    struct tevent_fd *fd_event;
    struct nbus_ctx *nbus_ctx;
};

static void async_listen_done(struct tevent_context *ev,
                              struct tevent_fd *fd_event, uint16_t flags,
                              void *private_data);

errno_t async_listen(TALLOC_CTX *mem_ctx, struct tevent_context *ev,
                     struct nbus_ctx *nbus_ctx)
{
    struct tevent_req *req;
    struct async_listen_state *state;
    int ret;

    req = tevent_req_create(mem_ctx, &state, struct async_listen_state);
    if (req == NULL) {
        LOG(LOG_CRIT, "tevent_req_create() failed.");
        ret = ENOMEM;
        goto done;
    }

    state->nbus_ctx = nbus_ctx;
    state->fd_event
        = tevent_add_fd(ev, state, nbus_get_sock_fd(state->nbus_ctx),
                        TEVENT_FD_READ, async_listen_done, req);
    if (state->fd_event == NULL) {
        LOG(LOG_CRIT, "tevent_add_fd() failed.");
        ret = ENOMEM;
        goto done;
    }

    ret = EOK;

done:
    /*
        if (ret == EOK) {
            tevent_req_done(req);
        } else {
            tevent_req_error(req, ret);
        }
    */

    tevent_req_poll(req, ev);
    return ret;
}

static void async_listen_done(struct tevent_context *ev,
                              struct tevent_fd *fd_event, uint16_t flags,
                              void *private_data)
{
    struct tevent_req *req = talloc_get_type(private_data, struct tevent_req);
    struct async_listen_state *state
        = tevent_req_data(req, struct async_listen_state);

    LOG(LOG_CRIT, "async_listen_done() -- asi mohu cist");
    talloc_zfree(state->fd_event);

    /*
        if (ret == EOK) {
            tevent_req_done(req);
        } else {
            ret = errno;
            LOG(LOG_CRIT, "connect failed [%d][%s].\n", ret, strerror(ret));
            tevent_req_error(req, ret);
        }
    */
    tevent_req_done(req);
}
