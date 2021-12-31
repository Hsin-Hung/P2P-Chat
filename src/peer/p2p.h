#ifndef _P2P_H_

#include <string>
#include <sys/poll.h>

extern struct pollfd fds[200];
extern int nfds;
extern int readfd;
extern int writefd;
extern int pipefd[2];
void *p2p_server_init(int port);
int p2p_connect(std::string peer, int port);
void broadcast(std::string msg);
bool recv_all(int socket, void *buffer, size_t length);
bool send_all(int socket, void *buffer, size_t length);

#endif