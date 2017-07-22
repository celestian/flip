#ifndef __SQL_PRIVATE_H__
#define __SQL_PRIVATE_H__

#include <sqlite3.h>

struct sql_ctx {
    sqlite3 *db;
};

#endif /* __SQL_PRIVATE_H__ */