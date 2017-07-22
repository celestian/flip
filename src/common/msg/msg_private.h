#ifndef __MSG_PRIVATE_H__
#define __MSG_PRIVATE_H__

#include <time.h>

#include <talloc.h>

struct msg_ctx {
    char *sender;
    char *recepient;
    time_t timestamp;
    enum message_type message_type;
    void *data;
};

#endif /* __MSG_PRIVATE_H__ */