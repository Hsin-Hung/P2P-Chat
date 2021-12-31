#include "group.h"
#include <algorithm>
#include "../../include/httplib.h"
#include "../../include/json.hpp"

std::atomic_int64_t next_id{0};
std::vector<Group> groups;

Group::Group(Peer creator) : creator{creator}
{
    peers.push_back(creator);
    id = next_id++;
};

Group *find_group(int id)
{

    auto group_iter = std::find_if(groups.begin(), groups.end(), [&](const Group &g)
                                   { return g.id == id; });
    return &(*group_iter);
}

void Group::join_group(Peer peer)
{

    peers.push_back(peer);
}

bool Group::is_creator(Peer peer)
{
    return peer == creator;
}

void Group::init()
{

    nlohmann::json j;
    int n = peers.size();
    Peer from_peer;
    for (int i = 0; i < n; i++)
    {
        from_peer = peers[0];
        if (peers.size() > 0)
        {
            peers.erase(peers.begin());
        }
        j = peers;
        httplib::Client cli(from_peer.ip, from_peer.port);
        httplib::Params params;
        params.emplace("peers", j.dump());
        cli.Post("/p2p", params);
    }
}

void to_json(nlohmann::json &j, const Group &g)
{

    j["creator"] = g.creator;
    j["peers"] = g.peers;
    j["id"] = g.id;
}
void from_json(const nlohmann::json &j, Group &g)
{

    j.at("creator").get_to(g.creator);
    j.at("peers").get_to<std::vector<Peer>>(g.peers);
    j.at("id").get_to(g.id);
}

std::ostream &operator<<(std::ostream &os, const Group &g)
{
    os << "id: " << g.id << " creator: " << g.creator;
    return os;
}