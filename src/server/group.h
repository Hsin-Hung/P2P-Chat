#ifndef _GROUP_H_
#define _GROUP_H_

#include <vector>
#include <string>
#include <atomic>
#include "peer.h"
#include <iostream>

class Group
{
public:
    Peer creator;
    std::vector<Peer> peers;
    int id;
    Group(){};
    Group(Peer creator);
    void init();
    void join_group(Peer peer);
    bool is_creator(Peer peer);
    friend std::ostream &operator<<(std::ostream &os, const Group &g);
};

void to_json(nlohmann::json &j, const Group &g);
void from_json(const nlohmann::json &j, Group &g);
Group *find_group(int id);
extern std::vector<Group> groups;
extern std::atomic_int64_t next_id;

#endif
