#include <curl/curl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "src/common/json/json.h"
#include "src/common/utils/logs.h"

char *jsmn_strerror(int errnum)
{
    char *err_msg;

    switch (errnum) {
    case (JSMN_ERROR_NOMEM):
        err_msg = "Not enough tokens were provided.";
        break;
    case (JSMN_ERROR_INVAL):
        err_msg = "Invalid character inside JSON string.";
        break;
    case (JSMN_ERROR_PART):
        err_msg = "The string is not a full JSON packet, more bytes expected.";
        break;
    default:
        err_msg = "Invalid JSMN_ERROR.";
    }

    return err_msg;
}

errno_t jsmn_error(int errnum)
{
    errno_t ret;

    switch (errnum) {
    case (JSMN_ERROR_NOMEM):
        ret = ENOMEM;
        break;
    case (JSMN_ERROR_INVAL):
        ret = EIO;
        break;
    case (JSMN_ERROR_PART):
        ret = EAGAIN;
        break;
    default:
        ret = ENOENT;
    }

    return ret;
}

char *jsmntype_str(int type_number)
{
    char *msg;

    switch (type_number) {
    case (JSMN_UNDEFINED):
        msg = "<JSMN_UNDEFINED 0>";
        break;
    case (JSMN_OBJECT):
        msg = "<JSMN_OBJECT 1>";
        break;
    case (JSMN_ARRAY):
        msg = "<JSMN_ARRAY 2>";
        break;
    case (JSMN_STRING):
        msg = "<JSMN_STRING 3>";
        break;
    case (JSMN_PRIMITIVE):
        msg = "<JSMN_PRIMITIVE 4>";
        break;
    default:
        msg = "<err: Invalid jsmntype_t>";
    }

    return msg;
}

errno_t get_json_token_count(char *json, int *tokens_count)
{
    jsmn_parser parser;
    int ret;

    jsmn_init(&parser);

    ret = jsmn_parse(&parser, json, strlen(json), NULL, 0);
    if (ret < 0) {
        LOG(LOG_ERR, "jsmn_parse() failed. [%d | %s]", ret, jsmn_strerror(ret));
        ret = jsmn_error(ret);
        goto done;
    }
    *tokens_count = ret;

    ret = EOK;

done:
    return ret;
}

bool equal_json_key(const char *json, jsmntok_t *token, const char *key)
{
    if (token->type == JSMN_STRING
        && (int)strlen(key) == token->end - token->start
        && strncmp(json + token->start, key, token->end - token->start) == 0) {
        return true;
    }

    return false;
}

double get_json_double(char *json, jsmntok_t *token)
{
    if (token->type != JSMN_PRIMITIVE) {
        return 0;
    }

    const char *a = &json[token->start];
    char *b = &json[token->end];

    return strtold(a, &b);
}

int get_json_int(char *json, jsmntok_t *token)
{
    if (token->type != JSMN_PRIMITIVE) {
        return 0;
    }

    const char *a = &json[token->start];
    char *b = &json[token->end];

    return strtol(a, &b, 10);
}