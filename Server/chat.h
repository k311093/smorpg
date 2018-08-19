#ifndef _CHAT_H_
#define _CHAT_H_
struct CHAT_DATA {
	SA_IN addr;
	char *data;
};

void *Chat(void *arg);
#endif
