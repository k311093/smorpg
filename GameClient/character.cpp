#include "character.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>

using namespace std;

Character character[255];

Character::Character() {
	animation = false;
	facing = FACE_DOWN;
	x = 0;
	y = 0;
	width = 0;
	height = 0;
	motionbuf = NULL;
	facedata = NULL;
	frames = NULL;
	frame = 0;
	chattime = 0;
	who = -1;
}

Character::~Character() {
	int i;
	if(frames != NULL) {
		delete [] frames;
		frames = NULL;
	}
	if(motionbuf != NULL) {
		for(i=0;i<nummotions;i++) {
			delete [] motionbuf[i];
		}
		delete motionbuf;
		motionbuf = NULL;
	}
}

void Character::Load(char *filename) {
	int i,j;
	int numframes;
	FILE *fp;
	char buf[80];
	if((fp = fopen(filename,"r")) == NULL) return;
	fgets(buf,80,fp);
	strtok(buf," \r\n");
	charbmp.SetImage(buf);
	facedata = new Coordinate[4];
	fscanf(fp,"%d",&width);
	fscanf(fp,"%d",&height);
	fscanf(fp,"%d",&facedata[FACE_RIGHT].bmpX);
	fscanf(fp,"%d",&facedata[FACE_RIGHT].bmpY);
	fscanf(fp,"%d",&facedata[FACE_LEFT].bmpX);
	fscanf(fp,"%d",&facedata[FACE_LEFT].bmpY);
	fscanf(fp,"%d",&facedata[FACE_DOWN].bmpX);
	fscanf(fp,"%d",&facedata[FACE_DOWN].bmpY);
	fscanf(fp,"%d",&facedata[FACE_UP].bmpX);
	fscanf(fp,"%d",&facedata[FACE_UP].bmpY);
	fscanf(fp,"%d",&nummotions);
	motionbuf = new Motion*[nummotions];
	frames = new int[nummotions];
	for(i=0;i<nummotions;i++) {
		fscanf(fp,"%d",&numframes);
		motionbuf[i] = new Motion[numframes];
		frames[i] = numframes;
		for(j=0;j<numframes;j++) {
			fscanf(fp,"%d",&motionbuf[i][j].bmpX);
			fscanf(fp,"%d",&motionbuf[i][j].bmpY);
		}
		for(j=0;j<numframes;j++) {
			fscanf(fp,"%d",&motionbuf[i][j].dx);
			fscanf(fp,"%d",&motionbuf[i][j].dy);
		}
	}
	fclose(fp);
}

void Character::Draw(HDC hdc) {
	if(facedata == NULL) return;
	HDC memdc;
	HBITMAP hbmp;
	RECT rect;
	int boxwidth,boxheight;
	POINT chatwindow[7];

	memdc = CreateCompatibleDC(hdc);
	hbmp = CreateCompatibleBitmap(hdc,width,height);
	SelectObject(memdc,hbmp);
	SetBkMode(hdc,TRANSPARENT);
	if(!animation) {
		charbmp.Draw(memdc,0,0,facedata[facing].bmpX*width,facedata[facing].bmpY*height,width,height);
		TransparentBlt(hdc,x,y-32,width,height,memdc,0,0,width,height,GetPixel(memdc,0,0));
	}
	else {
		charbmp.Draw(memdc,0,0,motionbuf[currentMotion][frame].bmpX*width,
			motionbuf[currentMotion][frame].bmpY*height,width,height);
		TransparentBlt(hdc,x,y-32,width,height,memdc,0,0,width,height,GetPixel(memdc,0,0));
	}
	if(chating) {
		rect.left = 0;
		rect.right = 100;
		rect.top = 0;
		rect.bottom = 0;
		DrawText(hdc,chat,strlen(chat),&rect,DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL | DT_NOPREFIX );
		boxwidth = rect.right;
		boxheight = rect.bottom;
		rect.left = x - (boxwidth - 32)/2;
		rect.top = y - (boxheight+32);
		rect.right += rect.left;
		rect.bottom += rect.top;
		chatwindow[0].x = rect.left-3;
		chatwindow[0].y = rect.top-6;
		chatwindow[1].x = rect.right+3;
		chatwindow[1].y = rect.top-6;
		chatwindow[2].x = rect.right+3;
		chatwindow[2].y = rect.bottom;
		chatwindow[3].x = rect.left + boxwidth/2 + 7;
		chatwindow[3].y = rect.bottom;
		chatwindow[4].x = rect.left + boxwidth/2 + 2;
		chatwindow[4].y = rect.bottom + 5;
		chatwindow[5].x = rect.left + boxwidth/2 + 2;
		chatwindow[5].y = rect.bottom;
		chatwindow[6].x = rect.left-3;
		chatwindow[6].y = rect.bottom;
		Polygon(hdc,chatwindow,7);

		rect.bottom -= 3;
		rect.top -= 3;

		DrawText(hdc,chat,strlen(chat),&rect,DT_WORDBREAK | DT_EDITCONTROL | DT_NOPREFIX );
	}
	DeleteObject(hbmp);
	DeleteDC(memdc);
}

void Character::PlayMotion(int motionNo) {
	currentMotion = motionNo;
	frame = 0;
	animation = true;
}

void Character::TimeProc() {
	if(facedata == NULL) return;
	if(animation)
	{
		x += motionbuf[currentMotion][frame].dx;
		y += motionbuf[currentMotion][frame].dy;
		if(frame == frames[currentMotion]-1) {
			animation = false;
			frame = 0;
			return;
		}
		frame++;
	}
	if(chating == true) {
		chattime--;
		if(!chattime) {
			chating = false;
		}
	}
}

void Character::Face(int vector) {
	facing = vector;
}

void Character::MoveDown() {
	if(!animation) PlayMotion(FACE_DOWN);
}

void Character::MoveLeft() {
	if(!animation) PlayMotion(FACE_LEFT);
}

void Character::MoveRight() {
	if(!animation) PlayMotion(FACE_RIGHT);
}

void Character::MoveUp() {
	if(!animation) PlayMotion(FACE_UP);
}

void Character::SetTileX(int ax) {
	x = ax*32;
}

void Character::SetTileY(int ay) {
	y = ay*32;
}

void Character::Chat(char *chat) {
	chattime = 30;
	strcpy(this->chat,chat);
	chating = true;
}