#include <algorithm>
#include <functional>
#include "group.h"
#include "httplib.h"
#include "json.hpp"

std::mutex group_id_mutex;
std::atomic_int64_t next_id{0};
std::vector<Group> groups;
std::array<std::mutex, 32> group_mutexes;

Group::Group(Peer creator) : creator{creator}, started{0}
{
    peers.push_back(creator);
    std::lock_guard<std::mutex> lock(group_id_mutex);
    id = std::to_string(next_id);
    ++next_id;
};

void add_new_group(Group new_group)
{
    std::lock_guard<std::mutex> lock(group_mutexes[std::stoi(new_group.id) % group_mutexes.size()]);
    groups.push_back(new_group);
}

Group *find_group(std::string id)
{
    std::lock_guard<std::mutex> lock(group_mutexes[std::stoi(id) % group_mutexes.size()]);
    auto group_iter = std::find_if(groups.begin(), groups.end(), [&](const Group &g)
                                   { return g.id == id; });
    return &(*group_iter);
}

bool Group::join_group(Peer peer)
{
    std::cout << peer << " join group !" << std::endl;

    if (!already_joined(peer))
    {
        std::lock_guard<std::mutex> lock(group_mutexes[std::stoi(id) % group_mutexes.size()]);
        peers.push_back(peer);
    }
    return true;
}

bool Group::is_creator(Peer &peer)
{
    return peer == creator;
}

bool Group::already_joined(Peer &peer)
{

    std::lock_guard<std::mutex> lock(group_mutexes[std::stoi(id) % group_mutexes.size()]);
    auto peer_iter = std::find_if(peers.begin(), peers.end(), [&](const Peer &p)
                                  { return p == peer; });
    return peer_iter != peers.end();
}

bool Group::init()
{

    nlohmann::json j;
    std::lock_guard<std::mutex> lock(group_mutexes[std::stoi(id) % group_mutexes.size()]);
    j["group"] = *this;
    std::vector<Peer> conn_peers{peers};
    int n = conn_peers.size();
    Peer from_peer;
    for (int i = 0; i < n; i++)
    {
        std::cout << "sending to " << i << std::endl;
        from_peer = conn_peers[0];
        if (conn_peers.size() > 0)
        {
            conn_peers.erase(conn_peers.begin());
        }
        j["conn_peers"] = conn_peers;
        httplib::Client cli(from_peer.ip, from_peer.port);
        httplib::Params params;
        params.emplace("init_info", j.dump());
        auto res = cli.Post("/p2p", params);
        if (res->status != 200)
            return false;
    }
    started = 1;
    return true;
}

bool Group::join_late(Peer peer)
{
    std::cout << peer << " join late !" << std::endl;
    std::mutex &mtx = group_mutexes[std::stoi(id) % group_mutexes.size()];
    nlohmann::json j;
    std::vector<Peer> conn_peers;
    mtx.lock();
    std::copy_if(peers.begin(), peers.end(), std::back_inserter(conn_peers), [&](Peer &p)
                 { return p != peer; });
    j["conn_peers"] = conn_peers;
    mtx.unlock();
    join_group(peer);
    mtx.lock();
    j["group"] = *this;
    mtx.unlock();
    httplib::Client cli(peer.ip, peer.port);
    httplib::Params params;
    params.emplace("init_info", j.dump());
    auto res = cli.Post("/p2p", params);
    if (res->status != 200)
        return false;
    return true;
}

bool Group::has_started()
{
    return started;
}

void to_json(nlohmann::json &j, const Group &g)
{

    j["creator"] = g.creator;
    j["peers"] = g.peers;
    j["id"] = g.id;
    j["started"] = g.started;
}
void from_json(const nlohmann::json &j, Group &g)
{

    j.at("creator").get_to(g.creator);
    j.at("peers").get_to<std::vector<Peer>>(g.peers);
    j.at("id").get_to(g.id);
    j.at("started").get_to(g.started);
}

std::ostream &operator<<(std::ostream &os, const Group &g)
{
    os << "id: " << g.id << " creator: " << g.creator << " started: " << g.started;
    return os;
}