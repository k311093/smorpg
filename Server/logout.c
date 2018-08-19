#include "logout.h"
#include "common.h"
#include "safelist.h"
#include "user.h"
#include "broadcast.h"
#include "packet.h"
#include "map.h"
#include "userno.h"

struct SAFELIST Users[MAP_MAX];

void *LogOut(void *arg) {
	struct PACKET pack;
	struct User usr;
	int i;
	SA_IN addr = *(SA_IN *)arg;
	for(i=0;i<MAP_MAX;i++) {
		if(FindItem(Users[i],&addr,&usr) == 1) {
			pack.who = usr.userno;
			pack.command = PACKET_LOGOUT;
			break;
		}
	}
	if(i == MAP_MAX) return NULL;
	BroadCastMap(addr,pack);
	DelUserno(usr.userno);
	DeleteItem(&Users[i],&addr);
	return NULL;
}
