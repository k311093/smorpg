#ifndef _CHARACTER_H_
#define _CHARACTER_H_

#define FACE_UP 3
#define FACE_DOWN 2
#define FACE_LEFT 1
#define FACE_RIGHT 0

#include "common.h"

void *CharacterMove(void *);
int IsMovable(int mapno, int x, int y);
void MoveMap(SA_IN addr, int oldmapno, int newmapno);

struct CHARACTER {
	char name[20];
	unsigned char mapno;
	int x,y;
	int facing;
};
#endif
