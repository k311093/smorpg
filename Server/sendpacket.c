#include "common.h"
#include "sendpacket.h"
#include "safelist.h"

extern int sendsocket;
extern pthread_mutex_t DelUser_Mutex;
extern SA_IN DelUser;
extern struct SAFELIST WaitingList;

struct SendStruct {
	void *packet;
	int len;
	SA_IN toaddr;
};

void *SendThread(void *arg);

void SendPacket(SA_IN toaddr, void *packet, int len, char reply) {
	struct SendStruct *sendpacket = malloc(sizeof(struct SendStruct));
	pthread_t thread_id;
	if(reply == 0) {
		sendto(sendsocket, (char *)packet, len,0 ,(SA *)&toaddr, sizeof(SA_IN));
	}
	else {
		sendpacket->packet = packet;
		sendpacket->len = len;
		memcpy(&sendpacket->toaddr,&toaddr,sizeof(SA_IN));
		pthread_create(&thread_id,NULL,SendThread,sendpacket);
	}
}

void *SendThread(void *arg) {
	struct SendStruct *sendpacket = (struct SendStruct *)arg;
	struct timespec delaytime;
	int i;

	delaytime.tv_sec = 0;
	delaytime.tv_nsec = DELAY_NANOSEC;

	for(i=0;i<TIMEOUT_RETRY;i++) {
		sendto(sendsocket, (char *)sendpacket->packet, sendpacket->len, 
			0, (SA *)&sendpacket->toaddr,sizeof(SA_IN));
		nanosleep(&delaytime,NULL);
		if(FindItem(WaitingList ,&(sendpacket->toaddr),NULL) == 0) {
			free(arg);
			return;
		}
	}	
	LOCK_DELUSR();
	memcpy(&DelUser,&sendpacket->toaddr,sizeof(SA_IN));
	free(arg);
	raise(SIGUSR1);
}
