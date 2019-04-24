#include "PerlinNoise.h"
#define _USE_MATH_DEFINES
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <math.h>
constexpr float PI = 3.141592653589793238463,
RandMult = 1.0 / RAND_MAX,//a
RandMultPI = RandMult * PI,
Noise3BaseValue = 0.5f / 0.55f;//basevalue = 0.5f / 55f
constexpr float PI_F = 3.14159265358979f;
const float MAX_NOISE_OUTPUT = 0.707;//sqrt(0.5)
const float NOISE_MULT = 1 / MAX_NOISE_OUTPUT;//multiply output by this to map from -1 to 1

Noise3::Noise3(GLuint seed)
{
	srand(seed);
	float rotation;
	float rotation2;
	float cosz;
	for (int i = 0; i < ArraySize; i += dimensions)
	{
		rotation = rand() * RandMult * (PI * 2);
		rotation2 = rand() * RandMult * PI - PI / 2;
		cosz = cos(rotation2);
		gradients[i] = sin(rotation) * cosz;//x
		gradients[i + 1] = cos(rotation) * cosz;//y
		gradients[i + 2] = sin(rotation2);//z
	}
}

// Compute Perlin noise at coordinates x, y
float Noise3::Evaluate(float x, float y, float z)
{
	//https://www.redblobgames.com/maps/terrain-from-noise/
	// Determine grid cell coordinates
	int x0 = floor(x), y0 = floor(y), z0 = floor(z);
	int x1 = x0 + 1, y1 = y0 + 1, z1 = z0 + 1;
	int cellx0 = x0 % NoiseScale,
		celly0 = y0 % NoiseScale,
		cellz0 = z0 % NoiseScale;
	if (cellx0 < 0) cellx0 += NoiseScale;
	if (celly0 < 0) celly0 += NoiseScale;
	if (cellz0 < 0) cellz0 += NoiseScale;
	int
		cellx1 = (cellx0 + 1) % NoiseScale,
		celly1 = (celly0 + 1) % NoiseScale,
		cellz1 = (cellz0 + 1) % NoiseScale;
	cellx0 *= StrideX;
	cellx1 *= StrideX;
	celly0 *= StrideY;
	celly1 *= StrideY;
	cellz0 *= StrideZ;
	cellz1 *= StrideZ;
	// Determine interpolation weights
	// Could also use higher order polynomial/s-curve here
	float
		sx0 = x - x0,
		sy0 = y - y0,
		sz0 = z - z0,
		sx1 = x - x1,
		sy1 = y - y1,
		sz1 = z - z1;

	// Interpolate between grid point gradients
	float n0, n1, ix0, ix1, iy0, iy1;//, value;
	//x0,y0,z0
	int //x03 = cellx0 * 3,
		//x13 = cellx1 * 3,
		y0scalet3 = celly0 * StrideY,
		y1scalet3 = celly1 * StrideY,
		z0scale2t3 = cellz0 * StrideZ,
		z1scale2t3 = cellz1 * StrideZ;
	float* ptr = gradients;
	float*
		ptrx0 = ptr + cellx0 * StrideX;
	float*
		ptrx1 = ptr + cellx1 * StrideX;
	//x0,y0,z0
	ptr = ptrx0 + celly0 + cellz0;
	n0 = sx0 * *ptr++ + sy0 * *ptr++ + sz0 * *ptr;
	//x1,y0,z0
	ptr = ptrx1 + celly0 + cellz0;
	n1 = sx1 * *ptr++ + sy0 * *ptr++ + sz0 * *ptr;
	//ix0 = Lerp(n0, n1, sx);
	ix0 = n0 + (n1 - n0) * sx0;
	//x0,y1,z0
	ptr = ptrx0 + celly1 + cellz0;
	n0 = sx0 * *ptr++ + sy1 * *ptr++ + sz0 * *ptr;
	//x1,y1,z0
	ptr = ptrx1 + celly1 + cellz0;
	n1 = sx1 * *ptr++ + sy1 * *ptr++ + sz0 * *ptr;
	//ix1 = Lerp(n0, n1, sx)
	ix1 = n0 + (n1 - n0) * sx0;
	//value = Lerp(ix0, ix1, sy);
	iy0 = ix0 + (ix1 - ix0) * sy0;
	//x0,y0,z1
	ptr = ptrx0 + celly0 + cellz1;
	n0 = sx0 * *ptr++ + sy0 * *ptr++ + sz1 * *ptr;
	//x1,y0,z1
	ptr = ptrx1 + celly0 + cellz1;
	n1 = sx1 * *ptr++ + sy0 * *ptr++ + sz1 * *ptr;
	//ix0 = Lerp(n0, n1, sx);
	ix0 = n0 + (n1 - n0) * sx0;
	//x0,y1,z1
	ptr = ptrx0 + celly1 + cellz1;
	n0 = sx0 * *ptr++ + sy1 * *ptr++ + sz1 * *ptr;
	//x1,y1,z1
	ptr = ptrx1 + celly1 + cellz1;
	n1 = sx1 * *ptr++ + sy1 * *ptr++ + sz1 * *ptr;
	//ix1 = Lerp(n0, n1, sx)
	ix1 = n0 + (n1 - n0) * sx0;
	//value = Lerp(ix0, ix1, sy);
	iy1 = ix0 + (ix1 - ix0) * sy0;
	return iy0 + (iy1 - iy0) * sz0;
}

Noise2::Noise2(GLuint seed)
{
	srand(seed);
	float rotation;
	for (int i = 0; i < ArraySize; i += dimensions)
	{
		rotation = rand() * RandMult * (PI * 2);
		gradients[i] = sin(rotation);//x
		gradients[i + 1] = cos(rotation);//y
	}
}
// Compute Perlin noise at coordinates x, y
//sources:
//https://en.wikipedia.org/wiki/Perlin_noise
//https://mzucker.github.io/html/perlin-noise-math-faq.html
//https://www.redblobgames.com/maps/terrain-from-noise/
float Noise2::Evaluate(float x, float y)
{
	// Determine grid cell coordinates
	int x0 = floor(x), y0 = floor(y);
	int x1 = x0 + 1, y1 = y0 + 1;
	int cellx0 = x0 % NoiseScale,
		celly0 = y0 % NoiseScale;
	if (cellx0 < 0) cellx0 += NoiseScale;
	if (celly0 < 0) celly0 += NoiseScale;
	int
		cellx1 = (cellx0 + 1) % NoiseScale * StrideX,
		celly1 = (celly0 + 1) % NoiseScale * StrideY;
	cellx0 *= StrideX;
	celly0 *= StrideY;
	// Determine interpolation weights
	// Could also use higher order polynomial/s-curve here
	float
		sx0 = x - x0,
		sy0 = y - y0,
		sx1 = x - x1,
		sy1 = y - y1;

	// Interpolate between grid point gradients
	float n0, n1, ix0, ix1;//, value;
	//x0,y0,z0
	int //x03 = cellx0 * 3,
		//x13 = cellx1 * 3,
		y0scalet3 = celly0 * StrideY,
		y1scalet3 = celly1 * StrideY;
	float* ptr = gradients;
	float*
		ptrx0 = ptr + cellx0;
	float*
		ptrx1 = ptr + cellx1;
	//x0,y0
	ptr = ptrx0 + celly0;
	n0 = sx0 * *ptr++ + sy0 * *ptr;
	//x1,y0
	ptr = ptrx1 + celly0;
	n1 = sx1 * *ptr++ + sy0 * *ptr;
	//ix0 = Lerp(n0, n1, sx);
	ix0 = n0 + (n1 - n0) * sx0;

	//x0,y1
	ptr = ptrx0 + celly1;
	n0 = sx0 * *ptr++ + sy1 * *ptr;
	//x1,y1
	ptr = ptrx1 + celly1;
	n1 = sx1 * *ptr++ + sy1 * *ptr;
	//ix1 = Lerp(n0, n1, sx)
	ix1 = n0 + (n1 - n0) * sx0;

	//value = Lerp(ix0, ix1, sy);
	return ix0 + (ix1 - ix0) * sy0;
}

static int Hash(int x, int y, int z)
{
	//https://github.com/Auburns/FastNoise_CSharp/blob/master/FastNoise.cs
	const int X_PRIME = 1619;
	const int Y_PRIME = 31337;
	const int Z_PRIME = 6971;
	const int seed = 1;
	int hash = seed ^ (X_PRIME * x) ^ (Y_PRIME * y) ^ (Z_PRIME * z);
	hash = hash * hash * hash * 60493;
	return (hash >> 13) ^ hash;
}

OptionNoise2::OptionNoise2(GLuint seed, float* OctaveWeights,const int Octaves, float min, float max, float Frequency)
{
	float TotalWeight = 0;
	BaseNoise = new Noise2(seed);//initialize base noise
	//sum weights
	for (int i = 0; i < Octaves; i++) 
	{
		TotalWeight += OctaveWeights[i];
	}

	//map weights
	float mid = (min + max) * .5f;
	this->OutputPlus = mid;
	float amplitude = max - mid;
	float mult = amplitude * NOISE_MULT / TotalWeight;
	for (int i = 0; i < Octaves; i++)
	{
		OctaveWeights[i] *= mult;
	}
	this->Octaves = Octaves;
	this->OctaveWeights = OctaveWeights;
	this->Frequency = Frequency;
}

float* OptionNoise2::Evaluate(int X, int Y, const int w, const int l)
{
	const int size = w * l;
	float* OutputValues = new float[size];
	float* ptr = OutputValues;
	int EndX = X + w;
	int EndY = Y + l;
	for(int aY = Y; aY < EndY; aY++)
	{
		for (int aX = X; aX < EndX; aX++)
		{
			float val = OutputPlus;
			float freq = Frequency;
			for (int i = 0; i < Octaves; i++)
			{
				val += OctaveWeights[i] * BaseNoise->Evaluate(aX * freq, aY * freq);
				freq *= 2.0f;
			}
			*ptr++ = val;
		}
	}
	return OutputValues;
}
