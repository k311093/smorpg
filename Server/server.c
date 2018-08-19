#include "common.h"
#include "sendpacket.h"
#include "user.h"
#include "safelist.h"
#include "packet.h"
#include "map.h"
#include "login.h"
#include "chat.h"
#include "broadcast.h"
#include "logout.h"
#include "userno.h"

extern int sendsocket, recvsocket;
extern pthread_mutex_t DelUser_Mutex;
extern SA_IN DelUser;
extern struct SAFELIST WaitingList;
extern struct SAFELIST Users[MAP_MAX];
extern int userno;
extern struct MAP Maps[MAP_MAX];

void KillUser(int arg);

struct LOGINDATA {
	SA_IN addr;
	char *name;
	char *password;
};

struct MOVEDATA {
	SA_IN addr;
	int direction;
};

int main() {
	struct LOGINDATA log;
	struct MOVEDATA mov;
	struct PACKET packet;
	struct CHAT_DATA chat;
	struct LOGIN *login_packet;
	struct CHAT *chat_packet;
	struct MOVE *move_packet;
	SA_IN temp_addr;
	int i;
	pthread_t thread;
	
	SA_IN server_addr, from_addr;
	int len = sizeof(SA_IN);

	bzero(&server_addr,sizeof(SA_IN));
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_family = AF_INET;

	signal(SIGUSR1,KillUser);
	pthread_mutex_init(&DelUser_Mutex,NULL);
	InitList(&WaitingList,0,sizeof(SA_IN));
	for(i=0;i<MAP_MAX;i++) {
		InitList(&Users[i],sizeof(struct User),sizeof(SA_IN));
	}
	sendsocket = socket(PF_INET,SOCK_DGRAM,0);
	recvsocket = socket(PF_INET,SOCK_DGRAM,0);
	
	if(bind(recvsocket,(SA *)&server_addr,sizeof(SA_IN)) != 0) {
		perror("Cannot Bind Port : ");
		return -1;
	}

	InitUserno();
	LoadMap(0);
	LoadMap(1);
	
	while(1) {
		recvfrom(recvsocket,(char *)&packet,sizeof(struct PACKET),0,(SA *)&from_addr,&len);
		switch(packet.command) {
			case PACKET_LOGIN:
				login_packet = (struct LOGIN *)packet.data;
				log.addr = from_addr;
				log.name = login_packet->username;
				log.password = login_packet->password;
				pthread_create(&thread,NULL,Login,&log);
				break;
			case PACKET_UP:
				mov.addr = from_addr;
				mov.direction = FACE_UP;
				pthread_create(&thread,NULL,CharacterMove,&mov);
				break;
			case PACKET_DOWN:
				mov.addr = from_addr;
				mov.direction = FACE_DOWN;
				pthread_create(&thread,NULL,CharacterMove,&mov);
				break;
			case PACKET_LEFT:
				mov.addr = from_addr;
				mov.direction = FACE_LEFT;
				pthread_create(&thread,NULL,CharacterMove,&mov);
				break;
			case PACKET_RIGHT:
				mov.addr = from_addr;
				mov.direction = FACE_RIGHT;
				pthread_create(&thread,NULL,CharacterMove,&mov);
				break;
			case PACKET_CHAT:
				chat.addr = from_addr;
				chat.data = packet.data;
				pthread_create(&thread,NULL,Chat,&chat);
				break;
			case PACKET_LOGOUT:
				temp_addr = from_addr;
				pthread_create(&thread,NULL,LogOut,&temp_addr);
				break;
		}
	}
}

void KillUser(int arg) {
	UNLOCK_DELUSR();	
}
