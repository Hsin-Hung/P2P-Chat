#ifndef GROUP_INIT_H
#define GROUP_INIT_H
#include <string>

class GroupInitializer
{
    std::string server_ip;
    int http_port, socket_port;

public:
    GroupInitializer(){};
    GroupInitializer(std::string server_ip, int http_port, int socket_port) : server_ip{server_ip}, http_port{http_port}, socket_port{socket_port} {};
    void init();
};

#endif