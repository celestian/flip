#include <stdio.h>
#include <stdlib.h>

#include "src/common/utils/logs.h"

int main(int argc, char *argv[])
{
    errno_t ret;

    printf("Hello World\n");

    ret = EXIT_SUCCESS;
    goto done;

done:
    exit(ret);
}
