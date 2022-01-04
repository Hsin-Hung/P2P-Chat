# P2P Group Chat

## Description
* A basic terminal p2p group chat implemented in C++. Each peer in a group will first contact a centralized server to retrieve the needed information to establish peer to peer connections. The peers will then establish direct connections with each others to start the group chat. 

## Build
* `make all`

## Run
* cloud run
  * centralized server:  `bin/server` 
  * peer: `bin/peer <server ip>`
* local run
  * centralized server:  `bin/server` 
  * peer: `bin/peer -local <party num>`, where `part num` should be from 1 to the number of parties and should be unique for each party