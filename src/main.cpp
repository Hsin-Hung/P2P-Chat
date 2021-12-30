#include <thread>
#include <iostream>
#include <sys/socket.h>
#include "p2p.h"

int main(int argc, char **argv)
{

    int i, valsend;
    std::string msg;
    if (argc >= 2)
    {
        std::cout << argv[1] << std::endl;
        i = std::stoi(argv[1]);
    }

    std::thread p2p_server(p2p_server_init, i);
    p2p_server.detach();    
    std::cout << "start chat" << std::endl;
    std::cin.ignore();
    if (std::stoi(argv[2]) == 8081)
    {
        std::cout << " conn to 8081 " << std::endl;
        p2p_connect("127.0.0.1", std::stoi(argv[2]));
    }

    while (1)
    {
        std::cout << "You: ";
        std::getline(std::cin, msg);
        broadcast(msg);
    }

    return 0;
}