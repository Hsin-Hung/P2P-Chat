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
                std::cout << "connected " <<  req.remote_addr << "  " << req.remote_port << std::endl;
                nlohmann::json j;
                j["groups"] = groups;
                std::cout << j.dump() << std::endl;
                res.status = 200;
                res.set_content(j.dump(), "application/json"); });

    svr.Post("/startgroup", [](const httplib::Request &req, httplib::Response &res)
             {
                 std::cout << "start group" << std::endl;
                 if (req.has_param("port") && req.has_param("name"))
                 {
                     nlohmann::json j;
                     auto port = req.get_param_value("port");
                     auto name = req.get_param_value("name");

                     Peer start_peer(name, req.remote_addr, std::stoi(port));
                     Group new_group(start_peer);
                     add_new_group(new_group);
                     j["group_id"] = new_group.id;
                     res.status = 200;
                     res.set_content(j.dump(), "application/json");
                 }
                 else
                 {
                     res.status = 400;
                     res.set_content("Wrong params!", "text/plain");
                 } });

    svr.Post("/init", [](const httplib::Request &req, httplib::Response &res)
             {
                 std::cout << "init" << std::endl;
                 if (req.has_param("port") && req.has_param("name") && req.has_param("group_id"))
                 {

                     auto port = req.get_param_value("port");
                     auto name = req.get_param_value("name");
                     auto group_id = req.get_param_value("group_id");

                     Peer init_peer(name, req.remote_addr, std::stoi(port));
                     Group *group = find_group(group_id);
                     if (group->is_creator(init_peer))
                     {
                         std::cout << "init success" << std::endl;
                         if(group->init()){
                             res.status = 200;
                            res.set_content("Init Succeed!", "text/plain");

                         }else{
                             res.status = 500;
                            res.set_content("Init Failed!", "text/plain");
                         }        
                         
                     }
                     else
                     {
                         res.status = 400;
                         res.set_content("You are not the group creator!", "text/plain");
                     }
                 }
                 else
                 {

                     res.status = 400;
                     res.set_content("Wrong params!", "text/plain");
                 } });

    svr.Post("/join", [](const httplib::Request &req, httplib::Response &res)
             {
                 std::cout << "join" << std::endl;
                 if (req.has_param("port") && req.has_param("name") && req.has_param("group_id"))
                 {

                    auto port = req.get_param_value("port");
                    auto name = req.get_param_value("name");
                    auto group_id = req.get_param_value("group_id");
                    std::cout << req.remote_addr << " " << port << " " << name << " want to join " << group_id << std::endl;
                    Peer new_peer(name, req.remote_addr, std::stoi(port));
                    Group *group = find_group(group_id);
                    
                     if (group->has_started())
                     {
                         if(group->join_late(new_peer)){
                            res.status = 200;
                            res.set_content("Join Succeed!", "text/plain");
                         }else{
                             res.status = 500;
                             res.set_content("Join Failed!", "text/plain");
                         }
                         
                         
                     }else if (group->join_group(new_peer)){
                         res.status = 200;
                         res.set_content("Join Succeed!", "text/plain");
                     }else{
                         res.status = 500;
                         res.set_content("Join Failed!", "text/plain");
                     }
                     
                 }
                 else
                 {
                     res.status = 400;
                     res.set_content("Wrong params!", "text/plain");
                 } });

    svr.listen("0.0.0.0", 8080);
    return 0;
}