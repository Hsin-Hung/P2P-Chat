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
#include "message.h"
#include "http.h"

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
    params.emplace("port", std::to_string(http_port));
    params.emplace("name", name);

    int cmd_complete{0};

    while (!cmd_complete)
    {
        if (auto res = cli.Get("/connect"))
        {
            if (res->status == 200)
            {
                std::string cmd;
                nlohmann::json j = nlohmann::json::parse(res->body);
                std::vector<Group> show_groups = j["groups"];

                std::cout << "Enter a group number to join, s to start a group, r to refresh list: \n";

                for (int i = 0; i < show_groups.size(); i++)
                {
                    std::cout << i << ": " << show_groups[i] << std::endl;
                }

                std::cin >> cmd;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                if (cmd == "s")
                {

                    res = cli.Post("/startgroup", params);
                    j = nlohmann::json::parse(res->body);
                    std::string new_group_id = j["group_id"];
                    std::cout << "new group with id: " << new_group_id << std::endl;
                    std::cout << "press any button to init chat: ";
                    std::cin.ignore();
                    params.emplace("group_id", new_group_id);
                    cli.Post("/init", params);
                    cmd_complete = 1;
                }
                else if (cmd == "r")
                {
                    continue;
                }
                else
                {
                    int group_index;
                    try
                    {
                        group_index = std::stoi(cmd);
                        if (group_index >= 0 && group_index < show_groups.size())
                        {

                            Group join_group = show_groups[group_index];
                            params.emplace("group_id", join_group.id);
                            cli.Post("/join", params);
                            cmd_complete = 1;
                        }
                        else
                        {
                            std::cout << "no such group!" << std::endl;
                        }
                    }
                    catch (std::invalid_argument &e)
                    {
                        // if no conversion could be performed
                        std::cout << "no conversion could be performed" << std::endl;
                    }
                    catch (std::out_of_range &e)
                    {
                        // if the converted value would fall out of the range of the result type
                        // or if the underlying function (std::strtol or std::strtoull) sets errno
                        // to ERANGE.
                        std::cout << "no conversion could be performed" << std::endl;
                    }
                }
            }
        }
        else
        {
            auto err = res.error();
        }
    }

    std::unique_lock<std::mutex> lock(http_mutex);
    std::cout << "waiting ..." << std::endl;
    http_cond.wait_for(lock,
                       std::chrono::seconds(1000),
                       []()
                       { return flag; });
    std::cout << "Start Chatting ..." << std::endl;

    while (1)
    {
        std::getline(std::cin, msg);
        if (broadcast(msg))
        {

            add_message("Me", msg);
        }
        else
        {

            std::cout << "broadcast incomplete !" << std::endl;
        }
    }

    return 0;
}