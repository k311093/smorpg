#include "common.h"

static unsigned char userbit[255] = {0,};
static pthread_mutex_t userbitMutex;

void InitUserno() {
	pthread_mutex_init(&userbitMutex,NULL);
}

int GetUserno() {
	pthread_mutex_lock(&userbitMutex);
	int i;
	int ret;
	int bit;
	for(i=0;i<255;i++) {
		if(userbit[i] != (unsigned char)'\xff') {
			for(bit=0;bit<8;bit++) {
				if((userbit[i] & (1<<bit)) == 0) {
					ret = i*8+bit;
					userbit[i] |= (1<<bit);
					pthread_mutex_unlock(&userbitMutex);
					return ret;
				}
			}
		}
	}
	pthread_mutex_unlock(&userbitMutex);
	return -1;
}

void DelUserno(int userno) {
	int index = userno/8;
	int bit = userno % 8;
	pthread_mutex_lock(&userbitMutex);
	userbit[index] &= ~(1<<bit);
	pthread_mutex_unlock(&userbitMutex);
	return;
}
