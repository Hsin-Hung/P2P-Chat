#ifndef _HTTP_H_
#define _HTTP_H_
#include <mutex>
#include <condition_variable>
#include "group.h"

#define HTTP_SERVER_PORT 8080

void *http_server_init(int port);
extern std::mutex http_mutex;
extern std::condition_variable http_cond;
extern bool flag;
extern Group current_group;

#endif