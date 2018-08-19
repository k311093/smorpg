#ifndef _MAP_H_
#define _MAP_H_

#include "bitmap.h"

#define MAP_MOVABLE	0x80
#define MAP_MAPMOVE 0x40

#pragma pack(1)
typedef struct {
	unsigned char tilex;
	unsigned char tiley;
	union _attr{
		char attbyte;
		struct _att {
			unsigned char mapno	  : 6;
			unsigned char mapmove : 1;
			unsigned char movable : 1;
		} attbit;
	} attribute;
} MapData;
#pragma pack()

class Map {
public:
	Map();
	~Map();
	MapData GetData(int x, int y);
	void Draw(HDC hdc,int x, int y,int cx, int cy, int stx, int sty);
	void Save(char *filename);
	void Load(char *name);
	int GetWidth();
	int GetHeight();
	void SetImage(char *filename);
	void Create(int width, int height);
	void SetTile(int x,int y,int tilex,int tiley);
	void SetAttribute(int x, int y,char attribute);
	const char *GetImageName();
private:
	HDC memdc;
	HBITMAP bitmap;
	Bitmap bmp;
	int width;
	int height;
	char imagename[80];
	MapData **dat;
};
#endif