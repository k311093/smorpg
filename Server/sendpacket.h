#ifndef _SENDPACKET_H_
#define _SENDPACKET_H_

#include "common.h"

void SendPacket(SA_IN toaddr, void *packet, int len, char reply);

#define UNLOCK_DELUSR() pthread_mutex_unlock(&DelUser_Mutex)
#define LOCK_DELUSR() pthread_mutex_lock(&DelUser_Mutex)
#define DELAY_NANOSEC 50000000L
#define TIMEOUT_RETRY 10

#endif
