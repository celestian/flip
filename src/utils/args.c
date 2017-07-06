#include <argp.h>
#include <talloc.h>

#include "./config.h"
#include "src/utils/args.h"
#include "src/utils/errors.h"
#include "src/utils/logs.h"
#include "src/utils/utils.h"

#define FLIPD_TAG "flipd | platform root daemon"
#define CRAWLER_TAG "flip_crawler | daemon for data crawling"
#define COLLECTOR_TAG "flip_collector | daemon for collecting data from crawler"

const char *argp_program_version = PACKAGE_STRING;
const char *argp_program_bug_address = PACKAGE_BUGREPORT;
static char flipd_args_doc[] = "<config_file>";
static char args_doc[] = "<identity_name> <pid_file> <root_ipc>";
static struct argp_option options[] = { { 0 } };

static error_t flipd_parse_opt(int key, char *arg, struct argp_state *state)
{
    struct flipd_args_ctx *args = state->input;

    switch (key) {
    case ARGP_KEY_ARG:
        if (state->arg_num > 1) {
            argp_usage(state);
        }

        if (state->arg_num == 0) {
            args->config_file = arg;
        }

        break;

    case ARGP_KEY_END:
        if (state->arg_num < 1) {
            argp_usage(state);
        }

        break;

    default:
        return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

static error_t worker_parse_opt(int key, char *arg, struct argp_state *state)
{
    struct worker_args_ctx *args = state->input;

    switch (key) {
    case ARGP_KEY_ARG:
        if (state->arg_num > 3) {
            argp_usage(state);
        }

        if (state->arg_num == 0) {
            args->identity_name = arg;
        } else if (state->arg_num == 1) {
            args->pid_file = arg;
        } else if (state->arg_num == 2) {
            args->root_ipc = arg;
        }
        break;

    case ARGP_KEY_END:
        if (state->arg_num < 3) {
            argp_usage(state);
        }
        break;

    default:
        return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

errno_t parse_flipd_args(TALLOC_CTX *mem_ctx, int argc, char *argv[],
                         struct flipd_args_ctx **_args)
{
    struct flipd_args_ctx args;
    errno_t ret;

    struct argp argp
        = { options, flipd_parse_opt, flipd_args_doc, FLIPD_TAG, 0, 0, 0 };

    args.config_file = NULL;

    ret = argp_parse(&argp, argc, argv, 0, 0, &args);
    if (ret != EOK) {
        LOG(LOG_ERR, "argp_parse() failed.");
        goto done;
    }

    *_args = talloc_memdup(mem_ctx, &args, sizeof(args));
    ret = EOK;

done:
    return ret;
}

errno_t parse_worker_args(TALLOC_CTX *mem_ctx, int argc, char *argv[],
                          enum daemon_type daemon_type,
                          struct worker_args_ctx **_args)
{
    struct worker_args_ctx args;
    errno_t ret;

    struct argp argp[]
        = { { options, worker_parse_opt, args_doc, CRAWLER_TAG, 0, 0, 0 },
            { options, worker_parse_opt, args_doc, COLLECTOR_TAG, 0, 0, 0 } };

    args.identity_name = NULL;
    args.pid_file = NULL;
    args.root_ipc = NULL;

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
        ret = EINVAL;
        goto done;
    }

    *_args = talloc_memdup(mem_ctx, &args, sizeof(args));
    ret = EOK;

done:
    return ret;
}
