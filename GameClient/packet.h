#ifndef _PACKET_H_
#define _PACKET_H_

enum COMMAND {
        PACKET_LOGIN,
        PACKET_LOGOUT,
        PACKET_CHAT,
        PACKET_MOVE,
        PACKET_SENDFILE,
		PACKET_OK,
		PACKET_NOTOK,
		PACKET_LOGINSUCCESS,
		PACKET_DOWN,
		PACKET_UP,
		PACKET_LEFT,
		PACKET_RIGHT,
};

struct CHARACTER {
	char name[20];
	unsigned char mapno;
	int x,y;
	int facing;
};

struct PACKET {
        unsigned short command;
		unsigned short who;
		unsigned long packno;
        char data[51];
};

struct LOGIN {
        char username[17];
        char password[9];
};

struct CHAT {
        char data[51];
};

struct MOVE {
		unsigned char mapno;
		int mapx;
		int mapy;
		int direction;
		unsigned short anim;
};

struct USER {
		int userno;
		struct CHARACTER Char;
};
#endif
