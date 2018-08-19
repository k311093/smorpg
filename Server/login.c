#include "common.h"
#include "safelist.h"
#include "packet.h"
#include <string.h>
#include "user.h"
#include "map.h"
#include "character.h"
#include "userno.h"

extern int sendsocket,recvsocket;
extern struct SAFELIST Users[MAP_MAX];

struct LOGINSUCCESS {
	int userno;
	struct CHARACTER Char;
};

struct LOGINDATA {
	SA_IN addr;
	char *name;
	char *password;
};

void *Login(void *arg) {
	struct LOGINDATA *log = arg;
	FILE *fp;
	char buf[80];
	int mapno;
	int len;
	int userno;
	int mapx,mapy;
	SA_IN addr;
	char *username;
	char *password;
	struct PACKET pack,tmppack;
	struct LOGINSUCCESS finduser;
	struct MOVE *packet_move;
	struct SAFELISTDATA *cur;
	struct User user;

	memcpy(&addr,&log->addr,sizeof(SA_IN));

	username = log->name;
	password = log->password;

	strcpy(buf,"./account/");
	strcat(buf,username);
	if((fp = fopen(buf,"r")) == NULL) {
		pack.command = PACKET_NOTOK;
		sendto(sendsocket,(char *)&pack,sizeof(struct PACKET),0,(SA *)&addr,sizeof(SA_IN));
		return NULL;
	}
	fgets(buf,80,fp);
	strtok(buf,"\r\n");
	if(strcmp(password,buf) != 0) {
		pack.command = PACKET_NOTOK;
		sendto(sendsocket,(char *)&pack,sizeof(struct PACKET),0,(SA *)&addr,sizeof(SA_IN));
		fclose(fp);
		return NULL;
	}
	userno = GetUserno();
	fscanf(fp,"%d",&mapno);
	fscanf(fp,"%d",&mapx);
	fscanf(fp,"%d",&mapy);
	finduser.userno = userno;
	strcpy(finduser.Char.name,username);
	finduser.Char.mapno = mapno;
	finduser.Char.x = mapx;
	finduser.Char.y = mapy;
	finduser.Char.facing = FACE_DOWN;
	pack.command = PACKET_LOGINSUCCESS;
	memcpy(pack.data,&finduser,sizeof(struct User));
	sendto(sendsocket,(char *)&pack,sizeof(struct PACKET),0,(SA *)&addr,sizeof(SA_IN));
	sleep(1);
	user.userno = userno;
	memcpy(&user.addr,&addr,sizeof(SA_IN));
	memcpy(&user.Char,&finduser.Char,sizeof(struct CHARACTER));
	pack.command = PACKET_MOVE;
	pack.who = userno;
	packet_move = (struct MOVE *)pack.data;
	packet_move->mapno = mapno;
	packet_move->mapx = mapx;
	packet_move->mapy = mapy;
	packet_move->direction = FACE_DOWN;
	packet_move->anim = 255;
	InsertItem(&Users[mapno],&addr,&user);
	LIST_LOCK(Users[mapno]);
	cur = Users[mapno].head;
	packet_move = (struct MOVE *)tmppack.data;
	while(cur != NULL) {
		if(((struct User *)cur->data)->userno != userno) { 
			packet_move->mapno = mapno;
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
	LIST_UNLOCK(Users[mapno]);
	return NULL;
}
