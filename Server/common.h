#ifndef _COMMON_H_
#define _COMMON_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>

#define SA_IN struct sockaddr_in
#define SA struct sockaddr
#define SERVER_PORT 32767
#define CLIENT_PORT 32768

#endif
