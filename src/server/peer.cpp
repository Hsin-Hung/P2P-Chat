#include "peer.h"

bool Peer::operator==(const Peer &peer) const
{

    return peer.name == name && peer.ip == ip && peer.port == port;
}
bool Peer::operator!=(const Peer &peer) const
{

    return !(*this == peer);
}

void to_json(nlohmann::json &j, const Peer &p)
{
    j["name"] = p.name;
    j["ip"] = p.ip;
    j["port"] = p.port;
}
void from_json(const nlohmann::json &j, Peer &p)
{
    j.at("name").get_to(p.name);
    j.at("ip").get_to(p.ip);
    j.at("port").get_to(p.port);
}

std::ostream &operator<<(std::ostream &os, const Peer &g)
{

    os << g.ip << "/" << g.port << "/" << g.name;
    return os;
}