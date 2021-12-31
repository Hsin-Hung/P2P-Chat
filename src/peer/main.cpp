#include "../../include/httplib.h"
#include "../../include/json.hpp"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <sys/socket.h>
#include <vector>
#include <string>
#include <ios>
#include <limits>
#include "p2p.h"
#include "../server/peer.h"
#include "../server/group.h"
#include "chat.h"

std::mutex mymutex;
std::condition_variable mycond;
bool flag = false;

void *http_server_init(int port)
{

    httplib::Server svr;

    svr.Post("/p2p", [](const httplib::Request &req, httplib::Response &res)
             { 

        if (req.has_param("peers")){
            std::unique_lock<std::mutex> lock(mymutex);
            auto peers = req.get_param_value("peers");
            nlohmann::json j = nlohmann::json::parse(peers);
            std::vector<Peer> conn_peers = j.get<std::vector<Peer>>();
            for(auto peer : conn_peers){
                std::cout << "connect to " << peer.ip << ":" << peer.port << std::endl;
                p2p_connect(peer.ip, peer.port - 1);
            }

        } 

        {
            std::lock_guard<std::mutex> lock(mymutex);
            flag = true;
            std::cout << "notify ..." << std::endl;
            mycond.notify_one();
        }
                 
        res.set_content("connect!", "application/json"); });

    std::cout << "listen on port " << port << std::endl;
    svr.listen("0.0.0.0", port);
}

int main(int argc, char **argv)
{

    int socket_port, http_port, valsend;
    std::string msg, name;
    if (argc >= 3)
    {
        socket_port = std::stoi(argv[1]);
        http_port = std::stoi(argv[2]);
    }

    std::thread http_server(http_server_init, http_port);
    http_server.detach();
    std::thread p2p_server(p2p_server_init, socket_port);
    p2p_server.detach();

    std::cout << "Enter your name: " << std::endl;
    std::cin >> name;

    httplib::Client cli("localhost", 8080);
    httplib::Params params;
    std::string my_ip = "127.0.0.1";
    params.emplace("ip", my_ip);
    params.emplace("port", std::to_string(http_port));
    params.emplace("name", name);

    if (auto res = cli.Get("/connect"))
    {
        std::string cmd_str;
        int cmd;
        nlohmann::json j = nlohmann::json::parse(res->body);
        std::vector<Group> show_groups = j["groups"];

        std::cout << "Enter a group number to join or 0 to start a group: ";

        for (int i = 0; i < show_groups.size(); i++)
        {
            std::cout << i + 1 << ", " << show_groups[i] << std::endl;
        }

        std::cin >> cmd;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (cmd == 0)
        {

            res = cli.Post("/startgroup", params);
            j = nlohmann::json::parse(res->body);
            int new_group_id = j["group_id"].get<int>();
            std::cout << "new group with id: " << new_group_id << std::endl;
            std::cout << "press any button to init chat: ";
            std::cin.ignore();
            params.emplace("group_id", std::to_string(new_group_id));
            cli.Post("/init", params);
        }
        else if (cmd >= 1 && cmd <= show_groups.size())
        {

            Group join_group = show_groups[cmd - 1];
            params.emplace("group_id", std::to_string(join_group.id));
            cli.Post("/join", params);
        }
    }

    std::unique_lock<std::mutex> lock(mymutex);
    std::cout << "waiting ..." << std::endl;
    mycond.wait_for(lock,
                    std::chrono::seconds(1000),
                    []()
                    { return flag; });
    std::cout << "Start Chatting ..." << std::endl;

    while (1)
    {
        std::getline(std::cin, msg);
        broadcast(msg);
        chat.add_message("Me", msg);
    }

    return 0;
}