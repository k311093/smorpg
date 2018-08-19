#include "map.h"
#include "common.h"
#include <string.h>

extern struct MAP Maps[MAP_MAX];

void LoadMap(int mapno) {
	FILE *fp;
	char filename[81];
	int width=0,height=0,i;
	sprintf(filename,"./map/map%d.mpd",mapno);
	if((fp = fopen(filename,"rb")) == NULL) {
		fprintf(stderr,"Cannot Open File %s\n",filename);
		return;
	}
	fread(filename,80,1,fp);
	fread(&width,sizeof(int),1,fp);
	fread(&height,sizeof(int),1,fp);
	Maps[mapno].width = width;
	Maps[mapno].height = height;
	Maps[mapno].data = (struct MAPDATA **)malloc(sizeof(struct MAPDATA *)*width);
	for(i=0;i<width;i++) {
		Maps[mapno].data[i] = (struct MAPDATA *)malloc(sizeof(struct MAPDATA)*height);
		fread(Maps[mapno].data[i],sizeof(struct MAPDATA)*height,1,fp);
	}
	fclose(fp);
}

void UnLoadMap(int mapno) {
	int i;
	for(i=0;i<Maps[mapno].width;i++) {
		free(Maps[mapno].data[i]);
	}
	free(Maps[mapno].data);
}
