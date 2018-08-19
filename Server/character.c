#include "common.h"
#include "safelist.h"
#include "packet.h"
#include "user.h"
#include "map.h"
#include "character.h"
#include "broadcast.h"

extern struct SAFELIST Users[MAP_MAX];
extern int sendsocket;
extern struct MAP Maps[MAP_MAX];

struct MOVEDATA {
	SA_IN addr;
	int direction;
};

struct LOGINSUCCESS {
	int userno;
	struct CHARACTER Char;
};

void *CharacterMove(void *arg) {
	int i;
	struct MOVEDATA *mov = (struct MOVEDATA *)arg;
	SA_IN addr;
	int direction;
	struct SAFELISTDATA *cur;
	struct User usr;
	struct MOVE *packet_move;
	struct PACKET pack;

	memcpy(&addr,&mov->addr,sizeof(SA_IN));
	direction = mov->direction;
	
	packet_move = (struct MOVE *)pack.data;
	
	for(i=0;i<MAP_MAX;i++) {
		if(FindItem(Users[i],&addr,&usr) == 1) {
			packet_move->mapno = usr.Char.mapno;
			packet_move->mapx = usr.Char.x;
			packet_move->mapy = usr.Char.y;
			packet_move->direction = direction;
			packet_move->anim = 255;
			pack.who = usr.userno;
			pack.command = PACKET_MOVE;
			switch(direction) {
				case FACE_UP:
					if(usr.Char.facing == FACE_UP && IsMovable(usr.Char.mapno,usr.Char.x,usr.Char.y-1)) {
						packet_move->anim = FACE_UP;
						usr.Char.y--;
					}
					break;
				case FACE_DOWN:
					if(usr.Char.facing == FACE_DOWN && IsMovable(usr.Char.mapno,usr.Char.x,usr.Char.y+1)) {
						packet_move->anim = FACE_DOWN;
						usr.Char.y++;
					}
					break;
				case FACE_LEFT:
					if(usr.Char.facing == FACE_LEFT && IsMovable(usr.Char.mapno,usr.Char.x-1,usr.Char.y)) {
						packet_move->anim = FACE_LEFT;
						usr.Char.x--;
					}
					break;
				case FACE_RIGHT:
					if(usr.Char.facing == FACE_RIGHT && IsMovable(usr.Char.mapno,usr.Char.x+1,usr.Char.y)) {
						packet_move->anim = FACE_RIGHT;
						usr.Char.x++;
					}
			}
			usr.Char.facing = direction;
			if(Maps[usr.Char.mapno].data[usr.Char.x][usr.Char.y].attribute & 0x40) {
				MoveMap(usr.addr,usr.Char.mapno,Maps[usr.Char.mapno].data[usr.Char.x][usr.Char.y].attribute & 0x3f);
				return NULL;
			}
			LIST_LOCK(Users[i]);
			cur = Users[i].head;
			while(cur != NULL) {
				sendto(sendsocket,(char *)&pack,sizeof(struct PACKET),0,(SA *)cur->key,sizeof(SA_IN));	
				cur = cur->next;
			}
			LIST_UNLOCK(Users[i]);
			SetItem(Users[i],&addr,&usr);
			return NULL;
		}
	}
}

int IsMovable(int mapno, int x, int y) {
	struct SAFELISTDATA *cur;
	if(!(Maps[mapno].data[x][y].attribute & 0x80)) {
		return 0;
	}
	LIST_LOCK(Users[mapno]);
	cur = Users[mapno].head;
	while(cur != NULL) {
		if((((struct User *)cur->data)->Char.x == x) && (((struct User *)cur->data)->Char.y == y)) {
			LIST_UNLOCK(Users[mapno]);
			return 0;
		}
		cur = cur->next;
	}
	LIST_UNLOCK(Users[mapno]);
	return 1;
}

void MoveMap(SA_IN addr, int oldmapno, int newmapno) {
	struct PACKET pack,tmppack;
	struct User usr;
	struct LOGINSUCCESS finduser;
	struct MOVE *packet_move;
	struct SAFELISTDATA *cur;
	
	FindItem(Users[oldmapno],&addr,&usr);
	pack.who = usr.userno;
	pack.command = PACKET_LOGOUT;
	BroadCastMap(addr,pack);
	DeleteItem(&Users[oldmapno],&addr);

	usr.Char.mapno = newmapno;
	usr.Char.x = 1;
	usr.Char.y = 1;
	usr.Char.facing = FACE_DOWN;
	strcpy(finduser.Char.name,usr.Char.name);
	memcpy(&finduser.Char,&usr.Char,sizeof(struct CHARACTER));
	finduser.userno = usr.userno;
	pack.command = PACKET_LOGINSUCCESS;
	memcpy(pack.data,&finduser,sizeof(struct User));
	sendto(sendsocket,(char *)&pack,sizeof(struct PACKET),0,(SA *)&addr,sizeof(SA_IN));
	sleep(1);

	pack.command = PACKET_MOVE;
	pack.who = usr.userno;
	packet_move = (struct MOVE *)pack.data;
	packet_move->mapno = newmapno;
	packet_move->mapx = 1;
	packet_move->mapy = 1;
	packet_move->direction = FACE_DOWN;
	packet_move->anim = 255;
	InsertItem(&Users[newmapno],&addr,&usr);
	LIST_LOCK(Users[newmapno]);
	cur = Users[newmapno].head;
	packet_move = (struct MOVE *)tmppack.data;
	while(cur != NULL) {
		if(((struct User *)cur->data)->userno != usr.userno) { 
			packet_move->mapno = newmapno;
			packet_move->mapx = ((struct User *)(cur->data))->Char.x;
			packet_move->mapy = ((struct User *)(cur->data))->Char.y;
			packet_move->direction = ((struct User *)(cur->data))->Char.facing;
			packet_move->anim = 255;
			tmppack.who = ((struct User *)cur->data)->userno;
			tmppack.command = PACKET_MOVE;
			sendto(sendsocket,(char *)&pack,sizeof(struct PACKET),0,(SA *)cur->key,sizeof(SA_IN));
			sendto(sendsocket,(char *)&tmppack,sizeof(struct PACKET),0,(SA *)&addr,sizeof(SA_IN));
		}
		cur = cur->next;
	}
	LIST_UNLOCK(Users[newmapno]);
	return;
}
