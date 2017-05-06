#ifndef __URL_PRIVATE_H__
#define __URL_PRIVATE_H__

#include <curl/curl.h>

#include "src/utils/data.h"

struct url_conn_ctx {
    CURL *curl_handle;
    struct string_ctx *string;
};

#endif /* __URL_PRIVATE_H__ */