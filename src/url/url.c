#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>
#include <talloc.h>

#include "src/url/url.h"
#include "src/url/url_private.h"
#include "src/utils/data.h"
#include "src/utils/errors.h"
#include "src/utils/logs.h"

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb,
                                  void *userp)
{
    size_t real_size = size * nmemb;

    if (real_size == 0) {
        return 0;
    }

    struct string_ctx *mem = (struct string_ctx *)userp;
    mem->data = talloc_strndup(mem->data, contents, real_size);
    if (mem->data == NULL) {
        fprintf(stderr,
                "Error [%d] [%s:%d] : Function talloc_strndup() failed.\n",
                ENOMEM, __FILE__, __LINE__);
        return 0;
    }
    mem->size = talloc_array_length(mem->data);

    return real_size;
}

errno_t url_global_init()
{
    CURLcode ret;

    ret = curl_global_init(CURL_GLOBAL_ALL);
    if (ret != CURLE_OK) {
        LOG(LOG_CRIT, "curl_global_init() failed [%d | %s]", ret,
            curl_easy_strerror(ret));
        return ENOENT;
    }

    return EOK;
}

void url_global_cleanup() { curl_global_cleanup(); }

errno_t url_init_ctx(TALLOC_CTX *mem_ctx, const char *url,
                     struct url_conn_ctx **_url_conn_ctx)
{
    TALLOC_CTX *tmp_ctx;
    struct url_conn_ctx *url_conn_ctx;
    errno_t ret;

    tmp_ctx = talloc_new(NULL);
    if (tmp_ctx == NULL) {
        return ENOMEM;
    }

    url_conn_ctx = talloc_zero(tmp_ctx, struct url_conn_ctx);
    if (url_conn_ctx == NULL) {
        ret = ENOMEM;
        goto done;
    }

    url_conn_ctx->string = talloc_zero(url_conn_ctx, struct string_ctx);
    if (url_conn_ctx->string == NULL) {
        ret = ENOMEM;
        goto done;
    }

    url_conn_ctx->curl_handle = curl_easy_init();
    curl_easy_setopt(url_conn_ctx->curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(url_conn_ctx->curl_handle, CURLOPT_WRITEFUNCTION,
                     WriteMemoryCallback);
    curl_easy_setopt(url_conn_ctx->curl_handle, CURLOPT_WRITEDATA,
                     (void *)url_conn_ctx->string);
    curl_easy_setopt(url_conn_ctx->curl_handle, CURLOPT_USERAGENT,
                     "libcurl-agent/1.0");

    *_url_conn_ctx = talloc_steal(mem_ctx, url_conn_ctx);
    ret = EOK;

done:
    talloc_free(tmp_ctx);
    return ret;
}

errno_t url_get_data(TALLOC_CTX *mem_ctx, struct url_conn_ctx *url_conn_ctx,
                     struct string_ctx **_chunk)
{
    TALLOC_CTX *tmp_ctx;
    CURLcode res;
    errno_t ret;

    tmp_ctx = talloc_new(NULL);
    if (tmp_ctx == NULL) {
        return ENOMEM;
    }

    res = curl_easy_perform(url_conn_ctx->curl_handle);
    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        goto done;
    }

    *_chunk = talloc_steal(mem_ctx, url_conn_ctx->string);

    ret = EOK;

    LOG(LOG_NOTICE, "Test C.");

done:
    talloc_free(tmp_ctx);
    return ret;
}

errno_t url_close(struct url_conn_ctx *url_conn_ctx_ctx)
{
    curl_easy_cleanup(url_conn_ctx_ctx->curl_handle);
    talloc_free(url_conn_ctx_ctx);

    return EOK;
}
