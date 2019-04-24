#include "LightMap.h"
#include "TextureConstants.h"
#include "stdio.h"
#pragma once
void GeneratePlaneXY(float x1, float y1, float z, float x2, float y2, float textureX, float textureY, float* fptr);
void GeneratePlaneXZ(float x1, float y, float z1, float x2, float z2, float textureX, float textureY, float* fptr);
void GeneratePlaneYZ(float x, float y1, float z1, float y2, float z2, float textureX, float textureY, float* fptr);
void GenerateCross(float x1, float y1, float z1, float x2, float y2, float z2, float textureX, float textureY, float* fptr);