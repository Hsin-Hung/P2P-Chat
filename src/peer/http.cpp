#include "http.h"
#include "p2p.h"
#include "peer.h"
#include "../../include/httplib.h"
#include "../../include/json.hpp"
#include <vector>
#include <string>
#include <iostream>

std::mutex http_mutex;
std::condition_variable http_cond;
bool flag = false;
Group current_group;

void *http_server_init(int port)
{

    httplib::Server svr;

    svr.Post("/p2p", [](const httplib::Request &req, httplib::Response &res)
             { 

        if (req.has_param("init_info")){
            auto peers = req.get_param_value("init_info");
            nlohmann::json j = nlohmann::json::parse(peers);
            current_group = j["group"].get<Group>();
            std::vector<Peer> conn_peers = j["conn_peers"].get<std::vector<Peer>>();
            for(auto peer : conn_peers){
                std::cout << "connecting to " << peer.name << " " << peer.ip << ":" << peer.port << std::endl;
                if(p2p_connect(peer.name, peer.ip, peer.port - 1)){
                    std::cout << peer.name << " joined the chat !" << std::endl;
                }else{
                    std::cout << peer.name << " failed to joined the chat !" << std::endl;
                }
            }


        {
            std::lock_guard<std::mutex> lock(http_mutex);
            flag = true;
            std::cout << "notify ..." << std::endl;
            http_cond.notify_one();
        }
            res.status = 200;
            res.set_content("P2P Connection Established!", "text/plain");
        }else{

            res.status = 500;
            res.set_content("Wrong params!", "text/plain");
            
        } });

    std::cout << "listen on port " << port << std::endl;
    svr.listen("0.0.0.0", port);
}