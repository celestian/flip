#ifndef __NBUS_PRIVATE_H__
#define __NBUS_PRIVATE_H__

struct nbus_ctx {
    const char *address;
    int sock_fd;
    int endpoint_id;
};

#endif /* __NBUS_PRIVATE_H__ */