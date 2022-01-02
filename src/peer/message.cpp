#include "message.h"
#include <iostream>

std::vector<Message> message_queue;
std::mutex msg_queue_mutex;

void add_message(std::string name, std::string msg)
{
    std::time_t result = std::time(nullptr);
    std::asctime(std::localtime(&result));

    std::lock_guard<std::mutex> lock(msg_queue_mutex);
    message_queue.push_back(Message(result, name, msg));
    std::cout << name << ":" << msg << std::endl;
}