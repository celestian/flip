#ifndef __BTCE_TICKER_H__
#define __BTCE_TICKER_H__

#include <talloc.h>

#include "src/common/json/jsmn/jsmn.h"
#include "src/common/utils/errors.h"

struct btce_ticker {
    char *pair;
    double high;
    double low;
    double avg;
    double vol;
    double vol_cur;
    double last;
    double buy;
    double sell;
    int updated;
};

errno_t parse_btc_e_ticker(TALLOC_CTX *mem_ctx, char *json,
                           struct btce_ticker **_ticker_data);

#endif /* __BTCE_TICKER_H__ */