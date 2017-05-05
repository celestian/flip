#include <argp.h>
#include <stdlib.h>

#include "config.h"

const char* argp_program_version = PACKAGE_STRING;
const char* argp_program_bug_address = PACKAGE_BUGREPORT;
static char doc[] = "flip_collector | daemon for collecting data from crawler";

static char args_doc[] = "<INPUT_IPC>";
static struct argp_option options[] = { { 0 } };

struct arguments {
    char* input_ipc;
};

static error_t parse_opt(int key, char* arg, struct argp_state* state)
{
    struct arguments* arguments = state->input;

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

int main(int argc, char** argv)
{
    struct arguments arguments;

    arguments.input_ipc = NULL;
    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    printf("OUTPUT_FILE = %s\n",
           arguments.output_ipc ? arguments.output_ipc : "NULL");

    exit(0);
}
