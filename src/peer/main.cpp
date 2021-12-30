#include "../../include/httplib.h"
#include "../../include/json.hpp"
#include <thread>
#include <iostream>
#include <sys/socket.h>
#include <vector>
#include <string>
#include <algorithm>
#include <utility>
#include "p2p.h"
#include "chat.h"

std::pair<std::string, int> get_ip_port(std::string ip_port){

    std::string delimiter = ":";
    size_t pos = ip_port.find(delimiter);
    std::string token = ip_port.substr(0, pos);
    ip_port.erase(0, pos + delimiter.length());

    return std::make_pair(token, std::stoi(ip_port));
}


void *http_server_init(int port){

    httplib::Server svr;

    svr.Post("/p2p", [](const httplib::Request &req, httplib::Response &res)
    { 

        if (req.has_param("peers")){

            auto peers = req.get_param_value("peers");
            nlohmann::json j = nlohmann::json::parse(peers);
            std::vector<std::string> ips = j;
            std::pair<std::string, int> ip_port;
            for(auto peer : ips){
                ip_port = get_ip_port(peer);
                std::cout << "connect to " << ip_port.first << ":" << ip_port.second << std::endl;
                p2p_connect(ip_port.first, ip_port.second - 1);
            }

        } 
                 
        res.set_content("connect!", "application/json"); });

    std::cout << "listen on port " << port << std::endl;
    svr.listen("0.0.0.0", port);

}

int main(int argc, char **argv)
{

    int socket_port, http_port, valsend;
    std::string msg;
    if (argc >= 3)
    {
        socket_port = std::stoi(argv[1]);
        http_port = std::stoi(argv[2]);
    }

    std::thread http_server(http_server_init, http_port);
    http_server.detach();
    std::thread p2p_server(p2p_server_init, socket_port);
    p2p_server.detach();  

    httplib::Client cli("localhost", 8080);
    httplib::Params params;
    std::string my_ip = "127.0.0.1:" + std::to_string(http_port);
    params.emplace("ip", my_ip);
    auto res = cli.Post("/connect", params);


    std::cout << "Join chat: ";
    std::cin.ignore();

    res = cli.Get("/join");

    std::cout << "start chat" << std::endl;
    while (1)
    {
        std::getline(std::cin, msg);
        broadcast(msg);
        chat.add_message("Me", msg);
    }

    return 0;
}