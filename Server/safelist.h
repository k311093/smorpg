#ifndef _SAFELIST_H_
#define _SAFELIST_H_

#include <pthread.h>

struct SAFELISTDATA {
	void *key;
	void *data;
	struct SAFELISTDATA *next;
};

struct SAFELIST {
	pthread_mutex_t mutex;
	int datasize;
	int keysize;
	struct SAFELISTDATA *head;
};

#define LIST_LOCK(X) pthread_mutex_lock(&X.mutex)
#define LIST_UNLOCK(X) pthread_mutex_unlock(&X.mutex)

int InitList(struct SAFELIST *list, int size, int keysize);
int ClearList(struct SAFELIST *list);
int FindItem(struct SAFELIST list, void *key, void *item);
int SetItem(struct SAFELIST list, void *key, void *item);
int InsertItem(struct SAFELIST *list, void *key, void *item);
int DeleteItem(struct SAFELIST *list, void *key);

#endif
