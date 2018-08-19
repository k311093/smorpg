#include <windows.h>

#ifndef _BITMAP_H_
#define _BITMAP_H_
class Bitmap {
public:
	Bitmap();
	Bitmap(const char *);
	void SetImage(const char *);
	void Draw(HDC,int, int,int xsrc = 0, int ysrc = 0,int cx=0,int cy=0);
	const char *GetFileName();
	int GetWidth();
	int GetHeight();
	~Bitmap();
private:
	char bmpFile[80];
	BITMAPFILEHEADER *pBMH;
	BITMAPINFO *pBMInfo;
	unsigned char *pBitData;
};

#endif