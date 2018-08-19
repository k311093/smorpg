#include "common.h"
#include "map.h"
#include "chat.h"
#include "broadcast.h"
#include "user.h"
#include "character.h"
#include "packet.h"
#include "safelist.h"

extern struct SAFELIST Users[MAP_MAX];
extern int sendsocket;

void *Chat(void *arg) {
	struct CHAT_DATA *dat = arg;
	SA_IN addr = dat->addr;
	char *chat = dat->data;
	struct PACKET pack;
	struct User usr;
	char data[51];
	int i;

	for(i=0;i<MAP_MAX;i++) {
		if(FindItem(Users[i],&addr,&usr) == 1) {
			strcpy(data,usr.Char.name);
			break;
		}
	}
	
	if(i == MAP_MAX) return NULL;

	strcat(data," : ");
	strcat(data,chat);
	pack.command = PACKET_CHAT;
	strcpy(pack.data,data);
	BroadCastMap(addr,pack);
	return NULL;
}
