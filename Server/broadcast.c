#include "common.h"
#include "user.h"
#include "safelist.h"
#include "map.h"
#include "packet.h"

extern struct SAFELIST Users[MAP_MAX];
extern int sendsocket;

void BroadCastMap(SA_IN who, struct PACKET pack) {
	int i;
	struct User usr;
	struct SAFELISTDATA *cur;

	for(i=0;i<MAP_MAX;i++) {
		if(FindItem(Users[i],&who,&usr) == 1) {
			pack.who = usr.userno;
			LIST_LOCK(Users[i]);
			cur = Users[i].head;
			while(cur != NULL) {
				sendto(sendsocket,(char *)&pack,sizeof(struct PACKET),0,(SA *)cur->key,sizeof(SA_IN));	
				cur = cur->next;
			}
			LIST_UNLOCK(Users[i]);
			return;
		}
	}
}
