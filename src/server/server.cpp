#include "../../include/httplib.h"
#include "../../include/json.hpp"
#include "group.h"
#include <string>
#include <vector>
#include <iostream>
#include <utility>

bool join_flag{false};

std::pair<std::string, int> get_ip_port(std::string ip_port)
{

    std::string delimiter = ":";
    size_t pos = ip_port.find(delimiter);
    std::string token = ip_port.substr(0, pos);
    ip_port.erase(0, pos + delimiter.length());

    return std::make_pair(token, std::stoi(ip_port));
}

int main()
{
    httplib::Server svr;
    svr.Get("/connect", [](const httplib::Request &req, httplib::Response &res)
             { 
                std::cout << "connected" << std::endl;
                nlohmann::json j;
                j["groups"] = groups;
                std::cout << j.dump() << std::endl;
                res.set_content(j.dump(), "application/json"); });

    svr.Post("/startgroup", [](const httplib::Request &req, httplib::Response &res)
             {
                 std::cout << "start group" << std::endl;
                 if (req.has_param("ip") && req.has_param("port") && req.has_param("name"))
                 {
                     nlohmann::json j;
                     auto ip = req.get_param_value("ip");
                     auto port = req.get_param_value("port");
                     auto name = req.get_param_value("name");

                     Peer start_peer(name, ip, std::stoi(port));
                     Group new_group(start_peer);
                     groups.push_back(new_group);
                     j["group_id"] = new_group.id;
                    res.set_content(j.dump(), "application/json");
                 } });

    svr.Post("/init", [](const httplib::Request &req, httplib::Response &res)
             {
                 std::cout << "init" << std::endl;
                 if (req.has_param("ip") && req.has_param("port") && req.has_param("name") && req.has_param("group_id"))
                 {

                     auto ip = req.get_param_value("ip");
                     auto port = req.get_param_value("port");
                     auto name = req.get_param_value("name");
                     auto group_id = req.get_param_value("group_id");

                     Peer init_peer(name, ip, std::stoi(port));
                     Group *group = find_group(std::stoi(group_id));
                     if (group->is_creator(init_peer))
                     {
                         std::cout << "init success" << std::endl;
                        group->init();
                        res.set_content("init fail", "application/json");
                     }else{
                        res.set_content("init successful", "application/json");
                     }
                 } });

    svr.Post("/join", [](const httplib::Request &req, httplib::Response &res)
            {

                std::cout << "join" << std::endl;
        if (req.has_param("ip") && req.has_param("port") && req.has_param("name") && req.has_param("group_id"))
        {

            auto ip = req.get_param_value("ip");
            auto port = req.get_param_value("port");
            auto name = req.get_param_value("name");
            auto group_id = req.get_param_value("group_id");
            std::cout << ip << " " << port << " " << name << " want to join " << group_id << std::endl;
            Peer new_peer(name, ip, std::stoi(port));
            Group *group = find_group(std::stoi(group_id));
            group->join_group(new_peer);
        }

        res.set_content("join!", "application/json"); });

    svr.listen("0.0.0.0", 8080);
    return 0;
}