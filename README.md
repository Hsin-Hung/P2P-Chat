# P2P Group Chat

## Description
* A basic p2p group chat implemented in C++. Each peer in a group will first contact a centralized server to retrieve the needed information to establish peer to peer connections. The peers will then establish direct connections with each others to start the group chat. 

## Dependencies

* https://github.com/nlohmann/json

* https://github.com/yhirose/cpp-httplib

## Build
* `make all`

## Run
* `bin/server` : the centralized server
* `bin/peer <server ip>` : the peer