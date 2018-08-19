#ifndef _CHARACTER_H_
#define _CHARACTER_H_
#include <windows.h>
#include "bitmap.h"
#include <vector>

using namespace std;

typedef struct {
	int bmpX;
	int bmpY;
} Coordinate;

typedef struct {
	int bmpX;
	int bmpY;
	int dx;
	int dy;
} Motion;

#define FACE_UP 3
#define FACE_DOWN 2
#define FACE_LEFT 1
#define FACE_RIGHT 0

class Character {
public:
	void MoveLeft();
	void MoveRight();
	void MoveUp();
	void MoveDown();
	void SetTileX(int x);
	void SetTileY(int y);
	operator =(Character src);
	Character();
	~Character();
	void Load(char *filename);
	void TimeProc();
	void Draw(HDC hdc);
	void PlayMotion(int motionNo);
	void Chat(char *chat);
	Coordinate *GetFaceData();
	void Face(int vector);
	int frame;
	bool chating;
	bool animation;
	int facing;
	int x,y;
	int nummotions;
	int width, height;
	int chattime;
	char chat[51];
	Bitmap charbmp;
	int *frames;
	Motion **motionbuf;
	int currentMotion;
	int who;
	Coordinate *facedata;
};

extern vector<Character> CharacterVector;

extern void CALLBACK TimerProc(HWND hWnd,UINT iMsg, UINT idEvent, DWORD dwTime);

#endif