#include <vector>
#include <string>

class Chat
{
public:
    std::vector<std::pair<std::string, std::string>> message_queue;
    Chat(){};
    void add_message(std::string party, std::string message);

};


extern Chat chat;