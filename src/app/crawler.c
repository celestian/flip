#include <argp.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "src/nbus/nbus.h"
#include "src/url/url.h"
#include "src/utils/daemon.h"
#include "src/utils/data.h"
#include "src/utils/errors.h"
#include "src/utils/logs.h"

const char* argp_program_version = PACKAGE_STRING;
const char* argp_program_bug_address = PACKAGE_BUGREPORT;
static char doc[] = "flip_crawler | daemon for data crawling";

static char args_doc[] = "<OUTPUT_IPC>";
static struct argp_option options[] = { { 0 } };

struct arguments {
    char* output_ipc;
};

static error_t parse_opt(int key, char* arg, struct argp_state* state)
{
    struct arguments* arguments = state->input;

    switch (key) {
    case ARGP_KEY_ARG:
        if (state->arg_num > 1) {
            argp_usage(state);
        }
        arguments->output_ipc = arg;
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

int main(int argc, char* argv[])
{
    TALLOC_CTX* mem_ctx;
    struct arguments arguments;
    struct url_conn_ctx* url_conn_ctx;
    struct nbus_ctx* nbus_ctx;
    struct string_ctx* chunk;
    errno_t ret;

    arguments.output_ipc = NULL;
    argp_parse(&argp, argc, argv, 0, 0, &arguments);
    printf("INPUT_FILE = %s\n",
           arguments.input_ipc ? arguments.input_ipc : "NULL");

    run_daemon("flip_crawler");

    mem_ctx = talloc_new(NULL);
    if (mem_ctx == NULL) {
        LOG(LOG_CRIT, "Critical failure: Not enough memory.");
        exit(EXIT_FAILURE);
    }

    ret = nbus_init_pub(mem_ctx, arguments.output_ipc, &nbus_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "Critical failure: Not enough memory.");
        exit(EXIT_FAILURE);
    }

    ret = url_global_init();
    if (ret != EOK) {
        LOG(LOG_CRIT, "curl init failed");
        exit(EXIT_FAILURE);
    }

    ret = url_init_ctx(mem_ctx, "https://btc-e.com/api/3/ticker/btc_usd",
                       &url_conn_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "Critical failure: Not enough memory.");
        exit(EXIT_FAILURE);
    }

    int i = 10;
    while (i > 0) {
        ret = url_get_data(mem_ctx, url_conn_ctx, &chunk);
        if (ret != EOK) {
            LOG(LOG_CRIT, "Critical failure: Not enough memory.");
            exit(EXIT_FAILURE);
        }

        ret = nbus_send(nbus_ctx, chunk->data, chunk->size);
        if (ret != EOK) {
            LOG(LOG_CRIT, "Critical failure: Not enough memory.");
            exit(EXIT_FAILURE);
        }

        sleep(1);
        i--;
    }

    ret = nbus_close(nbus_ctx);
    if (ret != EOK) {
        LOG(LOG_CRIT, "Critical failure: Not enough memory.");
        exit(EXIT_FAILURE);
    }

    url_global_cleanup();

    talloc_free(mem_ctx);

    exit(EXIT_SUCCESS);
}
