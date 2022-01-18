#include <iostream>
#include "groupInitializer.h"
#include "httplib.h"
#include "group.h"
#include "http.h"
#include "p2p.h"

volatile sig_atomic_t stop;

void inthand(int signum)
{
    stop = 1;
}

void GroupInitializer::init()
{
    std::string name, msg;
    std::cout << "Enter your name: " << std::endl;
    std::cin >> name;

    httplib::Client cli(server_ip, 8080);
    httplib::Params params;
    params.emplace("port", std::to_string(http_port));
    params.emplace("name", name);

    signal(SIGINT, inthand);

    while (1)
    {
        if (auto res = cli.Get("/connect"))
        {
            if (res->status == 200)
            {
                std::string cmd;
                nlohmann::json j = nlohmann::json::parse(res->body);
                std::vector<Group> show_groups = j["groups"];

                std::cout << "Enter the group number to join, 's' to start a group, 'r' to refresh the list: \n";

                for (auto i = 0; i < show_groups.size(); i++)
                {
                    std::cout << i << ": " << show_groups[i] << std::endl;
                }

                std::cin >> cmd;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                if (cmd == "r")
                    continue;

                if (cmd == "s")
                {

                    res = cli.Post("/startgroup", params);
                    if (res->status == 200)
                    {
                        j = nlohmann::json::parse(res->body);
                        std::string new_group_id = j["group_id"];
                        std::cout << "new group with id: " << new_group_id << std::endl;
                        std::cout << "press any button to init the group chat: ";
                        std::cin.ignore();

                        params.emplace("group_id", new_group_id);
                        res = cli.Post("/init", params);
                        if (res->status == 200)
                            break;
                    }

                    std::cout << res->body << std::endl;
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
                            break;
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

    std::cout << "Start Chatting (q to leave): " << std::endl;

    while (!stop)
    {
        std::getline(std::cin, msg);
        if (msg.compare("q") == 0)
            break;
        broadcast(name, msg);
    }
}