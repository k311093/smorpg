#include "bitmap.h"

Bitmap::Bitmap(const char *fileName) {
	strcpy(bmpFile,fileName);
	SetImage(fileName);
}

Bitmap::Bitmap() {
	pBMH = NULL;
}

void Bitmap::Draw(HDC hdc, int x, int y,int xsrc, int ysrc, int cx, int cy) {
	if(pBMH == NULL) {
		return;
	}
	if(cx == 0 || cy == 0)
		SetDIBitsToDevice(hdc,x,y,pBMInfo->bmiHeader.biWidth,abs(pBMInfo->bmiHeader.biHeight),0,0,0,abs(pBMInfo->bmiHeader.biHeight),pBitData,pBMInfo,DIB_RGB_COLORS);
	else
		SetDIBitsToDevice(hdc,x,y,cx,cy,xsrc,ysrc,0,abs(pBMInfo->bmiHeader.biHeight),pBitData,pBMInfo,DIB_RGB_COLORS);
}

Bitmap::~Bitmap() {
	free(pBMH);
}

void Bitmap::SetImage(const char *fileName) {
	if(pBMH != NULL) {
		free(pBMH);
	}
	DWORD dwFileSize, dwHighSize, dwBytesRead;
	HANDLE hFile;

	hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN,NULL);

	if(hFile == INVALID_HANDLE_VALUE) {
		pBMH = NULL;
		return;
	}
	strcpy(bmpFile,fileName);
	dwFileSize = GetFileSize(hFile,&dwHighSize);

	pBMH = (BITMAPFILEHEADER *)malloc(dwFileSize);

	ReadFile(hFile,pBMH,dwFileSize,&dwBytesRead,NULL);
	CloseHandle(hFile);

	pBMInfo = (BITMAPINFO *)(pBMH + 1);
	pBitData = (unsigned char *)pBMH + pBMH->bfOffBits;
}

int Bitmap::GetHeight() {
	if(pBMH == NULL) return 0;
	return abs(pBMInfo->bmiHeader.biHeight);
}

int Bitmap::GetWidth() {
	if(pBMH == NULL) return 0;
	return pBMInfo->bmiHeader.biWidth;
}

const char *Bitmap::GetFileName() {
	return (const char *)bmpFile;
}