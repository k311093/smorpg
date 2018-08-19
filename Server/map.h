#ifndef _MAP_H_
#define _MAP_H_

#define NUM_OF_MAP 1
#define MAP_MAX 255
struct MAPDATA {
	unsigned char tilex;
	unsigned char tiley;
	unsigned char attribute;
};

struct MAP {
	int width;
	int height;
	struct MAPDATA **data;
};

void LoadMap(int mapno);
void UnLoadMap(int mapno);

#endif
