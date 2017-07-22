#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/common/json/btc-e_ticker.h"
#include "src/common/json/jsmn/jsmn.h"
#include "src/common/json/json.h"
#include "src/common/utils/errors.h"
#include "src/common/utils/logs.h"

errno_t parse_btc_e_ticker(TALLOC_CTX *mem_ctx, char *json,
                           struct btce_ticker **_ticker_data)
{
    TALLOC_CTX *tmp_ctx;
    jsmn_parser parser;
    jsmntok_t *tokens;
    int tokens_count;
    size_t json_size;
    struct btce_ticker *ticker_data;
    int ret;

    tmp_ctx = talloc_new(NULL);
    if (tmp_ctx == NULL) {
        LOG(LOG_ERR, "talloc_new() failed.");
        return ENOMEM;
    }

    ticker_data = talloc(tmp_ctx, struct btce_ticker);
    if (ticker_data == NULL) {
        LOG(LOG_ERR, "talloc_new() failed.");
        return ENOMEM;
    }

    ret = get_json_token_count(json, &tokens_count);
    if (ret != EOK) {
        LOG(LOG_ERR, "get_json_token_count() failed. [%d | %s]", ret,
            strerror(ret));
        goto done;
    }

    tokens = talloc_array(tmp_ctx, jsmntok_t, tokens_count);
    if (tokens == NULL) {
        LOG(LOG_ERR, "talloc_array() failed.");
        ret = ENOMEM;
        goto done;
    }

    jsmn_init(&parser);
    json_size = strlen(json);

    ret = jsmn_parse(&parser, json, json_size, tokens, tokens_count);
    if (ret < 0) {
        LOG(LOG_ERR, "jsmn_parse() failed. [%d | %s]", ret, jsmn_strerror(ret));
        ret = jsmn_error(ret);
        goto done;
    }
    if (ret != tokens_count) {
        LOG(LOG_ERR, "jsmn_parse() didn't match tokens_count.");
        ret = EINVAL;
        goto done;
    }

    if (tokens[0].type != JSMN_OBJECT) {
        LOG(LOG_ERR, "JSON object expected.");
        ret = EINVAL;
        goto done;
    }

    if (tokens[1].type != JSMN_STRING) {
        LOG(LOG_ERR, "Wrong JSON structure.");
        ret = EINVAL;
        goto done;
    }

    ticker_data->pair = talloc_strndup(ticker_data, json + tokens[1].start,
                                       tokens[1].end - tokens[1].start);
    if (ticker_data->pair == NULL) {
        LOG(LOG_ERR, "talloc_strndup() failed.");
        return 0;
    }

    for (int i = 2; i < tokens_count; i++) {
        if (equal_json_key(json, &tokens[i], "high") == true) {
            ticker_data->high = get_json_double(json, &tokens[i + 1]);
        }

        if (equal_json_key(json, &tokens[i], "low") == true) {
            ticker_data->low = get_json_double(json, &tokens[i + 1]);
        }

        if (equal_json_key(json, &tokens[i], "avg") == true) {
            ticker_data->avg = get_json_double(json, &tokens[i + 1]);
        }

        if (equal_json_key(json, &tokens[i], "vol") == true) {
            ticker_data->vol = get_json_double(json, &tokens[i + 1]);
        }

        if (equal_json_key(json, &tokens[i], "vol_cur") == true) {
            ticker_data->vol_cur = get_json_double(json, &tokens[i + 1]);
        }

        if (equal_json_key(json, &tokens[i], "last") == true) {
            ticker_data->last = get_json_double(json, &tokens[i + 1]);
        }

        if (equal_json_key(json, &tokens[i], "buy") == true) {
            ticker_data->buy = get_json_double(json, &tokens[i + 1]);
        }

        if (equal_json_key(json, &tokens[i], "sell") == true) {
            ticker_data->sell = get_json_double(json, &tokens[i + 1]);
        }

        if (equal_json_key(json, &tokens[i], "updated") == true) {
            ticker_data->updated = get_json_int(json, &tokens[i + 1]);
        }
    }

    *_ticker_data = talloc_steal(mem_ctx, ticker_data);
    ret = EOK;

done:
    talloc_zfree(tmp_ctx);
    return ret;
}
