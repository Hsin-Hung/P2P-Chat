#ifndef _HTTP_H_
#define _HTTP_H_
#include <mutex>
#include <condition_variable>
#include "group.h"

void *http_server_init(int port);
extern std::mutex http_mutex;
extern std::condition_variable http_cond;
extern bool flag;
extern Group current_group;

#endif