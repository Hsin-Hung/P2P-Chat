#include "chat.h"
#include <iostream>

Chat chat;

void Chat::add_message(std::string party, std::string message)
{

    message_queue.push_back(std::make_pair(party, message));
    std::cout << party << ": " << message << std::endl;
    
}