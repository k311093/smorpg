#include "safelist.h"
#include "common.h"
#include "map.h"

pthread_mutex_t DelUser_Mutex;
SA_IN DelUser;
struct SAFELIST WaitingList;
struct SAFELIST Users[MAP_MAX];
struct MAP Maps[MAP_MAX];
int sendsocket, recvsocket;
