#ifndef _GROUP_H_
#define _GROUP_H_

#include <vector>
#include <string>
#include <atomic>
#include <iostream>
#include <mutex>
#include <array>
#include "peer.h"

class Group
{
public:
    Peer creator;
    std::vector<Peer> peers;
    std::string id;
    int started;
    Group(){};
    Group(Peer creator);
    bool init();
    bool join_late(Peer peer);
    bool join_group(Peer peer);
    bool is_creator(Peer &peer);
    bool has_started();
    bool already_joined(Peer &peer);
    friend std::ostream &operator<<(std::ostream &os, const Group &g);
};

void add_new_group(Group new_group);
void to_json(nlohmann::json &j, const Group &g);
void from_json(const nlohmann::json &j, Group &g);
Group *find_group(std::string id);
extern std::vector<Group> groups;
extern std::atomic_int64_t next_id;
extern std::mutex group_id_mutex;
extern std::array<std::mutex, 32> group_mutexes;
#endif
