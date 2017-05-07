#include <stdbool.h>
#include <unistd.h>

#include "src/utils/utils.h"

bool is_file_exist(const char *file_name)
{
    bool ret = false;

    if (access(file_name, F_OK) != -1) {
        ret = true;
    }

    return ret;
}
