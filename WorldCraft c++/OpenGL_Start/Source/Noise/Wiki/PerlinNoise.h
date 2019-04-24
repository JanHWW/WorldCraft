#include <iostream>
#include <stdio.h>
#include <glad/glad.h>//typedefs
#pragma once
class Noise3
{
private:
	static const int NoiseScale = 0xF;//must be a prime
	static const int NoiseScale2 = NoiseScale * NoiseScale;
	static const int dimensions = 3;
	static const int StrideX = dimensions;
	static const int StrideY = NoiseScale * StrideX;
	static const int StrideZ = NoiseScale * StrideY;
	static const int ArraySize = NoiseScale * StrideZ;
public:
	float gradients[ArraySize];
	float Evaluate(float x, float y, float z);
	Noise3(GLuint seed);
};
class Noise2
{
private:
	static const int NoiseScale = 0xF;//must be a prime
	static const int NoiseScale2 = NoiseScale * NoiseScale;
	static const int dimensions = 2;
	static const int StrideX = dimensions;
	static const int StrideY = NoiseScale * StrideX;
	static const int ArraySize = NoiseScale * StrideY;
public:
	float gradients[ArraySize];
	float Evaluate(float x, float y);
	Noise2(GLuint seed);
};
class OptionNoise2
{
public:
	OptionNoise2(GLuint seed, float* OctaveWeights, const int Octaves, const float min, const float max, const float Frequency);
	Noise2* BaseNoise;
	int Octaves;
	float* OctaveWeights;
	float OutputPlus;//the addition to the output to map it between min and max
	float Frequency;//the multiplier for the input coordinates
	float* Evaluate(int X, int Y, int w, int l);
	~OptionNoise2() 
	{
		delete BaseNoise;
		delete[] OctaveWeights;
	}
};