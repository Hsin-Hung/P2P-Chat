#ifndef _MESSAGE_H_
#define _MESSAGE_H_
#include <utility>
#include <vector>
#include <string>
#include <ctime>
#include <mutex>

class Message
{
public:
    time_t timestamp;
    std::string name;
    std::string msg;

    Message(time_t timestamp, std::string name, std::string msg) : timestamp{timestamp}, name{name}, msg{msg} {};
};

void add_message(std::string name, std::string msg);

extern std::mutex msg_queue_mutex;
extern std::vector<Message> message_queue;
#endif