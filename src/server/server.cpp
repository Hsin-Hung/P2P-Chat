#include "../../include/httplib.h"
#include "../../include/json.hpp"
#include <string>
#include <vector>
#include <utility>

std::vector<std::string> peers;
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
    svr.Post("/connect", [](const httplib::Request &req, httplib::Response &res)
             { 

                 if (req.has_param("ip")){

                    auto ip = req.get_param_value("ip");
                    std::cout << ip << " connected" << std::endl;
                    peers.push_back(ip);

                 } 
                 
                 res.set_content("connect!", "application/json"); });

    svr.Get("/join", [](const httplib::Request &req, httplib::Response &res)
            { 

                if(!join_flag){
                    join_flag = true;
                std::cout << "join" << std::endl;
                nlohmann::json j;
                int n = peers.size();
                for(int i = 0; i < n ; i++){
                    std::pair<std::string, int> ip_port = get_ip_port(peers[0]);
                    std::cout << ip_port.first << " " << ip_port.second << std::endl;
                    if (peers.size() > 0) {
                        peers.erase(peers.begin());
                    }
                    j = peers;
                    httplib::Client cli(ip_port.first, ip_port.second);
                    httplib::Params params;
                    params.emplace("peers", j.dump());
                    cli.Post("/p2p", params);
                }
                }

                 res.set_content("join!", "application/json"); });

    svr.listen("0.0.0.0", 8080);
    return 0;
}