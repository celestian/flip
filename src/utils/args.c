#include <argp.h>
#include <talloc.h>

#include "config.h"
#include "src/utils/args.h"
#include "src/utils/errors.h"
#include "src/utils/logs.h"
#include "src/utils/utils.h"

#define CRAWLER_TAG "flip_crawler | daemon for data crawling"
#define COLLECTOR_TAG "flip_collector | daemon for collecting data from crawler"

const char *argp_program_version = PACKAGE_STRING;
const char *argp_program_bug_address = PACKAGE_BUGREPORT;
static char args_doc[] = "<identity_name> <pid_file> <root_ipc>";
static struct argp_option options[] = { { 0 } };

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    struct worker_args_ctx *args = state->input;
    errno_t ret;

    switch (key) {
    case ARGP_KEY_ARG:
        if (state->arg_num > 3) {
            argp_usage(state);
        }
        if (state->arg_num == 0) {
            args->identity_name = arg;
        }
        if (state->arg_num == 1) {
            args->pid_file = arg;
        }
        if (state->arg_num == 2) {
            args->root_ipc = arg;
        }
        break;

    case ARGP_KEY_END:
        if (state->arg_num < 3) {
            argp_usage(state);
        }
        break;

    default:
        ret = ARGP_ERR_UNKNOWN;
        goto done;
    }

    ret = EOK;

done:
    return ret;
}

errno_t parse_worker_args(TALLOC_CTX *mem_ctx, int argc, char *argv[],
                          enum daemon_type daemon_type,
                          struct worker_args_ctx **_args)
{
    TALLOC_CTX *tmp_ctx;
    struct worker_args_ctx *args;
    errno_t ret;

    static struct argp argp[]
        = { { options, parse_opt, args_doc, CRAWLER_TAG, 0, 0, 0 },
            { options, parse_opt, args_doc, COLLECTOR_TAG, 0, 0, 0 } };

    tmp_ctx = talloc_new(NULL);
    if (tmp_ctx == NULL) {
        LOG(LOG_ERR, "talloc_new() failed.");
        return ENOMEM;
    }

    args = talloc(tmp_ctx, struct worker_args_ctx);
    if (args == NULL) {
        LOG(LOG_ERR, "talloc_new() failed.");
        ret = ENOMEM;
        goto done;
    }

    args->identity_name = NULL;
    args->root_ipc = NULL;

    switch (daemon_type) {
    case CRAWLER:
        ret = argp_parse(&argp[0], argc, argv, 0, 0, &args);
        if (ret != EOK) {
            LOG(LOG_ERR, "argp_parse() failed.");
            goto done;
        }

        break;

    case COLLECTOR:
        ret = argp_parse(&argp[1], argc, argv, 0, 0, &args);
        if (ret != EOK) {
            LOG(LOG_ERR, "argp_parse() failed.");
            goto done;
        }

        break;

    default:
        ret = ARGP_ERR_UNKNOWN;
        goto done;
    }

    *_args = talloc_steal(mem_ctx, args);
    ret = EOK;

done:
    talloc_free(tmp_ctx);
    return ret;
}