#include "Functions.h"
//xyz, xy,index
void GenerateCross(float x1, float y1, float z1, float x2, float y2, float z2, float textureX, float textureY, LightIntensity intensity, float* fptr)
{
	float textureX1 = textureX + TextSize;
	float textureY1 = textureY + TextSize;
	//xy to xy

	//00 to 11
	*fptr++ = x1;	*fptr++ = y1;	*fptr++ = z1; *fptr++ = textureX; *fptr++ = textureY;//000,00,0
	*fptr++ = x2;	*fptr++ = y2;	*fptr++ = z1; *fptr++ = textureX1; *fptr++ = textureY;//110,10,1
	*fptr++ = x1;	*fptr++ = y1;	*fptr++ = z2; *fptr++ = textureX; *fptr++ = textureY1;//001,01,2
	*fptr++ = x2;	*fptr++ = y2;	*fptr++ = z2; *fptr++ = textureX1; *fptr++ = textureY1;//111,11,3
	*fptr++ = x1;	*fptr++ = y1;	*fptr++ = z2; *fptr++ = textureX; *fptr++ = textureY1;//001,01,2
	*fptr++ = x2;	*fptr++ = y2;	*fptr++ = z1; *fptr++ = textureX1; *fptr++ = textureY;//110,10,1
	//01 to 10
	*fptr++ = x1;	*fptr++ = y2;	*fptr++ = z1; *fptr++ = textureX; *fptr++ = textureY;//010,00,0
	*fptr++ = x2;	*fptr++ = y1;	*fptr++ = z1; *fptr++ = textureX1; *fptr++ = textureY;//100,10,1
	*fptr++ = x1;	*fptr++ = y2;	*fptr++ = z2; *fptr++ = textureX; *fptr++ = textureY1;//011,01,2
	*fptr++ = x2;	*fptr++ = y1;	*fptr++ = z2; *fptr++ = textureX1; *fptr++ = textureY1;//101,11,3
	*fptr++ = x1;	*fptr++ = y2;	*fptr++ = z2; *fptr++ = textureX; *fptr++ = textureY1;//011,01,2
	*fptr++ = x2;	*fptr++ = y1;	*fptr++ = z1; *fptr++ = textureX1; *fptr++ = textureY;//100,10,1
}
void GeneratePlaneXY(float x1, float y1, float z, float x2, float y2, float textureX, float textureY, LightIntensity intensity, float* fptr)
{
	float textureX1 = textureX + TextSize;
	float textureY1 = textureY + TextSize;
	*fptr++ = x1;	*fptr++ = y1;	*fptr++ = z; *fptr++ = textureX; *fptr++ = textureY;//000,00,0
	*fptr++ = x2;	*fptr++ = y1;	*fptr++ = z; *fptr++ = textureX1; *fptr++ = textureY;//100,10,1
	*fptr++ = x1;	*fptr++ = y2;	*fptr++ = z; *fptr++ = textureX; *fptr++ = textureY1;//010,01,2
	*fptr++ = x2;	*fptr++ = y2;	*fptr++ = z; *fptr++ = textureX1; *fptr++ = textureY1;//110,11,3
	*fptr++ = x1;	*fptr++ = y2;	*fptr++ = z; *fptr++ = textureX; *fptr++ = textureY1;//010,01,2
	*fptr++ = x2;	*fptr++ = y1;	*fptr++ = z; *fptr++ = textureX1; *fptr++ = textureY;//100,10,1
}
void GeneratePlaneXZ(float x1, float y, float z1, float x2, float z2, float textureX, float textureY, LightIntensity intensity, float* fptr)
{
	float textureX1 = textureX + TextSize;
	float textureY1 = textureY + TextSize;
	*fptr++ = x1;	*fptr++ = y;	*fptr++ = z1; *fptr++ = textureX; *fptr++ = textureY;//000,00,0
	*fptr++ = x2;	*fptr++ = y;	*fptr++ = z1; *fptr++ = textureX1; *fptr++ = textureY;//100,10,1
	*fptr++ = x1;	*fptr++ = y;	*fptr++ = z2; *fptr++ = textureX; *fptr++ = textureY1;//001,01,2
	*fptr++ = x2;	*fptr++ = y;	*fptr++ = z2; *fptr++ = textureX1; *fptr++ = textureY1;//101,11,3
	*fptr++ = x1;	*fptr++ = y;	*fptr++ = z2; *fptr++ = textureX; *fptr++ = textureY1;//001,01,2
	*fptr++ = x2;	*fptr++ = y;	*fptr++ = z1; *fptr++ = textureX1; *fptr++ = textureY;//100,10,1
}
void GeneratePlaneYZ(float x, float y1, float z1, float y2, float z2, float textureX, float textureY, LightIntensity intensity, float* fptr)
{
	float textureX1 = textureX + TextSize;
	float textureY1 = textureY + TextSize;
	*fptr++ = x;	*fptr++ = y1;	*fptr++ = z1; *fptr++ = textureX; *fptr++ = textureY;//000,00,0
	*fptr++ = x;	*fptr++ = y2;	*fptr++ = z1; *fptr++ = textureX1; *fptr++ = textureY;//010,10,1
	*fptr++ = x;	*fptr++ = y1;	*fptr++ = z2; *fptr++ = textureX; *fptr++ = textureY1;//001,01,2
	*fptr++ = x;	*fptr++ = y2;	*fptr++ = z2; *fptr++ = textureX1; *fptr++ = textureY1;//011,11,3
	*fptr++ = x;	*fptr++ = y1;	*fptr++ = z2; *fptr++ = textureX; *fptr++ = textureY1;//001,01,2
	*fptr++ = x;	*fptr++ = y2;	*fptr++ = z1; *fptr++ = textureX1; *fptr++ = textureY;//010,10,1
}
