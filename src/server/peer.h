#ifndef _PEER_H_
#define _PEER_H_

#include "json.hpp"
#include <string>

class Peer
{
public:
    std::string name;
    std::string ip;
    int port;
    Peer(){};
    Peer(std::string name, std::string ip, int port) : name{name}, ip{ip}, port{port} {};
    Peer(const Peer &peer): name{peer.name}, ip{peer.ip}, port{peer.port} {};
    bool operator==( const Peer &peer ) const;
    bool operator!=( const Peer &peer ) const;
    friend std::ostream &operator<<(std::ostream &os, const Peer &g);
};

void to_json(nlohmann::json &j, const Peer &p);
void from_json(const nlohmann::json &j, Peer &p);

#endif