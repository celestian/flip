#include <argp.h>
#include <stdlib.h>
#include <unistd.h>

#include "config.h"
#include "src/nbus/nbus.h"
#include "src/utils/daemon.h"
#include "src/utils/errors.h"
#include "src/utils/logs.h"

const char *argp_program_version = PACKAGE_STRING;
const char *argp_program_bug_address = PACKAGE_BUGREPORT;
static char doc[] = "flip_collector | daemon for collecting data from crawler";

static char args_doc[] = "<INPUT_IPC>";
static struct argp_option options[] = { { 0 } };

struct arguments {
    char *input_ipc;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    struct arguments *arguments = state->input;

    switch (key) {
    case ARGP_KEY_ARG:
        if (state->arg_num > 1) {
            argp_usage(state);
        }
        arguments->input_ipc = arg;
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

static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };

int main(int argc, char **argv)
{
    TALLOC_CTX *mem_ctx;
    struct arguments arguments;
    struct nbus_ctx *nbus_ctx;
    struct string_ctx *chunk;
    errno_t ret;

    arguments.input_ipc = NULL;
    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    log_init("flip_collector");
    //    run_daemon("flip_collector");

    mem_ctx = talloc_new(NULL);
    if (mem_ctx == NULL) {
        LOG(LOG_CRIT, "Critical failure: Not enough memory.");
        exit(EXIT_FAILURE);
    }

    ret = nbus_init_sub(mem_ctx, arguments.input_ipc, &nbus_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "Critical failure: Not enough memory.");
        exit(EXIT_FAILURE);
    }

    int i = 20;
    while (i > 0) {

        errno_t nbus_recieve(TALLOC_CTX * mem_ctx, struct nbus_ctx * nbus_ctx,
                             struct string_ctx * *_chunk);

        ret = nbus_recieve(mem_ctx, nbus_ctx, &chunk);
        if (ret == EOK) {
            printf(">>> [%s]\n", chunk->data);
        } else {
            printf(">>> %d\n", ret);
        }
        /*
                if (ret != EOK) {
                    LOG(LOG_CRIT, "Critical failure: Not enough memory.");
                    exit(EXIT_FAILURE);
                }
        */

        sleep(2);
        i--;
    }

    ret = nbus_close(nbus_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "Critical failure: Not enough memory.");
        exit(EXIT_FAILURE);
    }

    talloc_free(mem_ctx);

    exit(EXIT_SUCCESS);
}
