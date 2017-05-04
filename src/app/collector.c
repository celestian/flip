#include "config.h"
#include <argp.h>
#include <stdlib.h>

const char* argp_program_version = PACKAGE_STRING;
const char* argp_program_bug_address = PACKAGE_BUGREPORT;

/* Program documentation. */
static char doc[] = "flip_collector | daemon for collecting data from crawler";

/* The options we understand. */
static struct argp_option options[]
    = { { "input-ipc", 'o', "FILE", 0,
          "Input .ipc file for reading data from crawler", 0 },
        { 0 } };

/* Used by main to communicate with parse_opt. */
struct arguments {
    char* input_ipc;
};

/* Parse a single option. */
static error_t parse_opt(int key, char* arg, struct argp_state* state)
{
    /* Get the input argument from argp_parse, which we
       know is a pointer to our arguments structure. */
    struct arguments* arguments = state->input;

    switch (key) {
    case 'o':
        arguments->input_ipc = arg;
        break;

    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

/* Our argp parser. */
static struct argp argp = { options, parse_opt, 0, doc, 0, 0, 0 };

int main(int argc, char** argv)
{
    struct arguments arguments;

    /* Default values. */
    arguments.input_ipc = NULL;

    /* Parse our arguments; every option seen by parse_opt will
       be reflected in arguments. */
    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    printf("OUTPUT_FILE = %s\n",
           arguments.input_ipc ? arguments.input_ipc : "NULL");

    exit(0);
}
