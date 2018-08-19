#ifndef _USER_H_
#define _USER_H_

#include "common.h"
#include "character.h"

struct User {
	int userno;
	SA_IN addr;
	struct CHARACTER Char;
};
#endif
