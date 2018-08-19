#include "map.h"
#include <stdio.h>
#include <cstring>

Map::Map() {
	width = height = 0;
	bitmap = NULL;
	memdc = NULL;
	dat = NULL;
	imagename[0] = 0;
}

Map::~Map() {
	int i;
	if(bitmap != NULL) {
		DeleteObject(bitmap);
		bitmap = NULL;
	}
	if(memdc != NULL) {
		DeleteDC(memdc);
		memdc = NULL;
	}
	if(dat == NULL) return;
	for(i=0;i<width;i++)
		delete [] dat[i];
	delete dat;
}

int Map::GetHeight() {
	return height;
}

int Map::GetWidth() {
	return width;
}

void Map::Create(int width, int height) {
	int i,j;
	if(bitmap != NULL) {
		DeleteObject(bitmap);
		bitmap = NULL;
	}
	if(memdc != NULL) {
		DeleteDC(memdc);
		memdc = NULL;
	}
	if(dat != NULL) {
		for(i=0;i<this->width;i++)
			delete [] dat[i];
		delete dat;
	}
	this->width = width;
	this->height = height;
	dat = new MapData *[width];
	for(i=0;i<width;i++) {
		dat[i] = new MapData[height];
	}
	for(i=0;i<width;i++) {
		for(j=0;j<height;j++) {
			dat[i][j].attribute.attbit.movable = true;
			dat[i][j].attribute.attbit.mapno = 0;
			dat[i][j].attribute.attbit.mapmove = false;
			dat[i][j].tilex = 0;
			dat[i][j].tiley = 0;
		}
	}
}

MapData Map::GetData(int x, int y) {
	return dat[x][y];
}

void Map::SetTile(int x,int y,int tilex, int tiley) {
	dat[x][y].tilex = tilex;
	dat[x][y].tiley = tiley;
}

void Map::Save(char *filename) {
	FILE *fp;
	int i;
	if((fp = fopen(filename,"wb")) == NULL) return;
	fseek(fp,0,SEEK_SET);
	fwrite(imagename,80,1,fp);
	fwrite(&width,sizeof(int),1,fp);
	fwrite(&height,sizeof(int),1,fp);
	for(i=0;i<width;i++) {
		fwrite(dat[i],sizeof(MapData)*height,1,fp);
	}
	fclose(fp);
}

void Map::Load(char *filename) {
	int i;
	FILE *fp;
	if((fp = fopen(filename,"rb")) == NULL) return;
	fseek(fp,0,SEEK_SET);
	fread(imagename,80,1,fp);
	bmp.SetImage(imagename);
	fread(&width,sizeof(int),1,fp);
	fread(&height,sizeof(int),1,fp);
	Create(width,height);
	for(i=0;i<width;i++) {
		fread(dat[i],sizeof(MapData)*height,1,fp);
	}
	fclose(fp);
}

void Map::SetImage(char *filename) {
	strcpy(imagename,filename);
}

const char *Map::GetImageName() {
	return imagename;
}

void Map::SetAttribute(int x, int y,char attribute) {
	dat[x][y].attribute.attbyte = attribute;
}

void Map::Draw(HDC hdc,int x, int y,int cx, int cy, int stx, int sty) {
	int i,j;
	if(width == 0) return;
	if(memdc == NULL) {
		memdc = CreateCompatibleDC(hdc);
		bitmap = CreateCompatibleBitmap(hdc,width*32,height*32);
		SelectObject(memdc,bitmap);
		for(i=0;i<width;i++) {
			for(j=0;j<height;j++) {
				bmp.Draw(memdc,i*32,j*32,GetData(i,j).tilex*32,GetData(i,j).tiley*32,32,32);
			}
		}
	}
	BitBlt(hdc,x,y,cx,cy,memdc,stx,sty,SRCCOPY);
}