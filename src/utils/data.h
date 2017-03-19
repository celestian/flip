#ifndef __DATA_H__
#define __DATA_H__

#include <stddef.h>

#include <talloc.h>

#include "src/utils/errors.h"

struct string_ctx {
    char* data;
    size_t size;
};

#endif /* __DATA_H__ */