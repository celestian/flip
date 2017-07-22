#ifndef __JSON_H__
#define __JSON_H__

#include <stdbool.h>

#include "src/common/json/jsmn/jsmn.h"
#include "src/common/utils/errors.h"

char *jsmn_strerror(int errnum);

errno_t jsmn_error(int errnum);

char *jsmntype_str(int type_number);

errno_t get_json_token_count(char *json, int *tokens_count);

bool equal_json_key(const char *json, jsmntok_t *token, const char *key);

double get_json_double(char *json, jsmntok_t *token);

int get_json_int(char *json, jsmntok_t *token);

#endif /* __JSON_H__ */
