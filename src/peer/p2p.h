#ifndef _P2P_H_
#define _P2P_H_
#include <string>
#include <sys/poll.h>
#define SOCKET_SERVER_PORT 8333

extern struct pollfd fds[200];
extern int nfds;
extern int pipefd[2];
void *p2p_server_init(int port);
bool p2p_connect(std::string name, std::string peer, int port);
bool broadcast(std::string msg);
bool recv_all(int socket, void *buffer, size_t length);
bool send_all(int socket, void *buffer, size_t length);

#endif