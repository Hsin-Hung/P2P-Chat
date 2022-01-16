#include "../../include/httplib.h"
#include "../../include/json.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <ios>
#include "p2p.h"
#include "peer.h"
#include "group.h"
#include "http.h"
#include "groupInitializer.h"

void start_bg_tasks(int http_port, int socket_port)
{
    std::thread http_server(http_server_init, http_port);
    http_server.detach();
    std::thread p2p_server(p2p_server_init, socket_port);
    p2p_server.detach();
}

int main(int argc, char **argv)
{

    int socket_port, http_port, valsend;
    std::string msg, name, server_ip;
    if (argc == 2)
    {
        // cloud run
        server_ip = argv[1];
        http_port = 8080;
        socket_port = 8333;
    }
    else if (argc == 3 && strcmp(argv[1], "-local") == 0 && std::atoi(argv[2]) > 0)
    {
        // local run
        server_ip = "127.0.0.1";
        http_port = 8080 + std::atoi(argv[2]) * 2;
        socket_port = http_port - 1;
    }
    else
    {
        std::cout << "Wrong arguments !" << std::endl;
        return 1;
    }
    start_bg_tasks(http_port, socket_port);

    GroupInitializer group_init(server_ip, http_port, socket_port);

    group_init.init();

    return 0;
}