#include <pthread.h>
#include <stdlib.h>
#include "safelist.h"

int InitList(struct SAFELIST *list,int size, int keysize) {
	pthread_mutex_init(&list->mutex,NULL);
	list->head = NULL;
	list->datasize = size;
	list->keysize = keysize;
	return 0;
} 

int FindItem(struct SAFELIST list, void *key, void *item) {
	struct SAFELISTDATA *cur;
	pthread_mutex_lock(&list.mutex);
	cur = list.head;
	if(cur == NULL) {
		pthread_mutex_unlock(&list.mutex);
		return 0;
	}
	while(cur != NULL) {
		if(!memcmp(cur->key,key,list.keysize)) {
			memcpy(item,cur->data,list.datasize);
			pthread_mutex_unlock(&list.mutex);
			return 1;
		}
		cur = cur->next;
	}
	pthread_mutex_unlock(&list.mutex);
	return 0;
}

int SetItem(struct SAFELIST list, void *key, void *item) {
	struct SAFELISTDATA *cur;
	pthread_mutex_lock(&list.mutex);
	cur = list.head;
	if(cur == NULL) {
		pthread_mutex_unlock(&list.mutex);
		return 0;
	}
	while(cur != NULL) {
		if(!memcmp(cur->key,key,list.keysize)) {
			memcpy(cur->data,item,list.datasize);
			pthread_mutex_unlock(&list.mutex);
			return 1;
		}
		cur = cur->next;
	}
	pthread_mutex_unlock(&list.mutex);
	return 0;
}

int InsertItem(struct SAFELIST *list, void *key, void *data) {
	struct SAFELISTDATA *cur,*tmp;
	pthread_mutex_lock(&list->mutex);
	cur = list->head;
	if(cur == NULL) {
		tmp = (struct SAFELISTDATA *)malloc(sizeof(struct SAFELISTDATA));
		tmp->data = (void *)malloc(list->datasize);
		tmp->key = (void *)malloc(list->keysize);
		memcpy(tmp->data,data,list->datasize);
		memcpy(tmp->key,key,list->keysize);
		tmp->next = NULL;
		list->head = tmp;
		pthread_mutex_unlock(&list->mutex);
		return 1;
	}
	if(!memcmp(cur->key,key,list->keysize)) {
		pthread_mutex_unlock(&list->mutex);
		return 0;
	}
	
	while(cur->next != NULL) {
		if(!memcmp(cur->key,key,list->keysize)) {
			pthread_mutex_unlock(&list->mutex);
			return 0;
		}
		cur = cur->next;
	}
	tmp = (struct SAFELISTDATA *)malloc(sizeof(struct SAFELISTDATA));
	tmp->data = (void *)malloc(list->datasize);
	tmp->key = (void *)malloc(list->keysize);
	memcpy(tmp->data,data,list->datasize);
	memcpy(tmp->key,key,list->keysize);
	tmp->next = cur->next;
	cur->next = tmp;
	pthread_mutex_unlock(&list->mutex);
	return 1;
}

int DeleteItem(struct SAFELIST *list, void *key) {
	struct SAFELISTDATA *cur,*prev;
	pthread_mutex_lock(&list->mutex);
	cur = list->head;
	if(cur == NULL) {
		pthread_mutex_unlock(&list->mutex);
		return 0;
	}
	if(!memcmp(cur->key,key,list->keysize)) {
		free(cur->data);
		free(cur->key);
		list->head = cur->next;
		free(cur);
		pthread_mutex_unlock(&list->mutex);
		return 1;
	}
	prev = cur;
	cur = cur->next;
	while(cur != NULL) {
		if(!memcmp(cur->key,key,list->keysize)) {
			prev->next = cur->next;
			free(cur->data);
			free(cur->key);
			free(cur);
			pthread_mutex_unlock(&list->mutex);
			return 1;
		}
		prev = cur;
		cur = cur->next;
	}
	return 0;
}

int ClearList(struct SAFELIST *list) {
	struct SAFELISTDATA *cur = list->head,*next;
	if(cur != NULL) {
		next = cur->next;
		while(next != NULL) {
			free(cur->data);
			free(cur->key);
			free(cur);
			cur = next;
			next = cur->next;
		}
	}
	list->head = NULL;
	pthread_mutex_destroy(&list->mutex);
	return 1;
}
