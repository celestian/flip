#include <talloc.h>
#include <tevent.h>
#include <unistd.h>

#include "src/common/nbus/nbus.h"
#include "src/common/utils/data.h"
#include "src/common/utils/logs.h"
#include "src/module/flipd/daemon_handler.h"

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
                wd_ctx->identity_name, wd_ctx->pid_file, wd_ctx->url_root_ipc);

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

// -----------------------------------------------------------------------------

// HEADERS

struct create_socket_state {
    struct tevent_fd *fd_event;
    const char *url_ipc;
    struct nbus_ctx *nbus_ctx;
};

struct run_daemon_state {
};

struct tevent_req *create_socket_send(TALLOC_CTX *mem_ctx,
                                      struct tevent_context *ev,
                                      const char *url_ipc);
static void create_socket_done(struct tevent_context *ev,
                               struct tevent_fd *fd_event, uint16_t flags,
                               void *private_data);

static void async_start_worker_daemon_done(struct tevent_req *subreq);

// -----------------------------------------------------------------------------

static void async_start_worker_daemon_done_2(struct tevent_context *ctx,
                                             struct tevent_immediate *im,
                                             void *private_data){
    LOG(LOG_CRIT, ">>> ano, jsme tu.")

}

errno_t
    async_start_worker_daemon(TALLOC_CTX *mem_ctx, const char *identity_name,
                              const char *pid_file, const char *url_root_ipc)
{
    TALLOC_CTX *tmp_ctx;
    struct tevent_context *event_ctx;
    struct tevent_immediate *im;
    struct run_daemon_state *state;
    struct tevent_req *req;
    struct tevent_req *subreq;
    errno_t ret;

    tmp_ctx = talloc_new(NULL);
    if (tmp_ctx == NULL) {
        LOG(LOG_ERR, "talloc_new() failed.");
        return ENOMEM;
    }

    // TODO: maybe it needs to be on mem_ctx
    event_ctx = tevent_context_init(tmp_ctx);
    if (event_ctx == NULL) {
        LOG(LOG_CRIT, "tevent_context_init() failed.");
        ret = ENOMEM;
        goto done;
    }

    req = tevent_req_create(mem_ctx, &state, struct run_daemon_state);
    if (req == NULL) {
        LOG(LOG_CRIT, "tevent_req_create() failed.");
        ret = ENOMEM;
        goto done;
    }

    subreq = create_socket_send(tmp_ctx, event_ctx, url_root_ipc);
    if (subreq == NULL) {
        LOG(LOG_CRIT, "create_socket_send() failed.");
        ret = ENOMEM;
        goto done;
    }

    char *command = talloc_asprintf(
        tmp_ctx, "/home/pcech/Projects/flip/x86_64/flip_crawler %s %s %s ",
        identity_name, pid_file, url_root_ipc);

    ret = system(command);
    if (ret != EOK) {
        LOG(LOG_CRIT, "system() failed.");
        ret = EIO;
        goto done;
    }
    talloc_zfree(command);

    LOG(LOG_CRIT, ">>> after command()");

    im = tevent_create_immediate(tmp_ctx);
    if (im == NULL) {
        LOG(LOG_CRIT, "tevent_create_immediate() failed.");
        ret = ENOMEM;
        goto done;
    }
    tevent_schedule_immediate(im, event_ctx, async_start_worker_daemon_done_2,
                              state);

    tevent_req_set_callback(subreq, async_start_worker_daemon_done, req);

    tevent_loop_wait(event_ctx);

    ret = EOK;

done:
    if (tmp_ctx != NULL) {
        talloc_zfree(tmp_ctx);
    }

    return ret;
}

static void async_start_worker_daemon_done(struct tevent_req *subreq)
{
    struct tevent_req *req
        = tevent_req_callback_data(subreq, struct tevent_req);
    int ret = EOK;
    //    struct run_daemon_state *state =
    //                tevent_req_data(req, struct run_daemon_state);
    /*


        // kill the timeout handler now that we got a reply
        talloc_zfree(state->connect_timeout);

        ret = sssd_async_connect_recv(subreq);
        talloc_zfree(subreq);
        if (ret != EOK) {
            DEBUG(SSSDBG_CRIT_FAILURE,
                  "sdap_async_sys_connect request failed: [%d]: %s.\n",
                  ret, sss_strerror(ret));
            goto fail;
        }
    */

    LOG(LOG_CRIT, ">>> async_start_worker_daemon_done() -- nieco nieco --");

    tevent_req_done(req);
    return;

    // fail:
    tevent_req_error(req, ret);
}

struct tevent_req *create_socket_send(TALLOC_CTX *mem_ctx,
                                      struct tevent_context *ev,
                                      const char *url_ipc)
{
    struct tevent_req *req;
    struct create_socket_state *state;
    int ret;

    req = tevent_req_create(mem_ctx, &state, struct create_socket_state);
    if (req == NULL) {
        LOG(LOG_CRIT, "tevent_req_create() failed.");
        return NULL;
    }

    state->url_ipc = url_ipc;
    ret = nbus_init_pair(mem_ctx, state->url_ipc, &state->nbus_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "nbus_init_pair() failed.");
        ret = EIO;
        goto done;
    }

    LOG(LOG_CRIT, ">>> nbus_init_pair()");

    state->fd_event
        = tevent_add_fd(ev, state, nbus_get_sock_fd(state->nbus_ctx),
                        TEVENT_FD_READ, create_socket_done, req);
    if (state->fd_event == NULL) {
        LOG(LOG_CRIT, "tevent_add_fd() failed.");
        ret = ENOMEM;
        goto done;
    }

    LOG(LOG_CRIT, ">>> tevent_add_fd()");

    return req;

done:
    if (ret == EOK) {
        tevent_req_done(req);
    } else {
        tevent_req_error(req, ret);
    }

    tevent_req_post(req, ev);
    return req;
}

static void create_socket_done(struct tevent_context *ev,
                               struct tevent_fd *fd_event, uint16_t flags,
                               void *private_data)
{
    struct tevent_req *req = talloc_get_type(private_data, struct tevent_req);
    /*
        struct create_socket_state *state
            = tevent_req_data(req, struct create_socket_state);
    */
    //    int ret;

    /*
        errno = 0;
        ret = connect(state->fd, (struct sockaddr *)&state->addr,
       state->addr_len);
        if (ret == -1) {
            ret = errno;
            if (ret == EALREADY || ret == EINPROGRESS || ret == EINTR) {
                return; // Try again later
}
}
*/

    LOG(LOG_CRIT, "create_socket_done() -- asi mohu cist");

    talloc_zfree(fd_event);

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
