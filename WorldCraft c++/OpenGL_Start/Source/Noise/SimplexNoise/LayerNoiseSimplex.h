#pragma once
#include "SimplexNoise.h"//base noise
#include <glad/glad.h>//typedefs
class LayerNoiseSimplex
{
public:
	LayerNoiseSimplex(GLuint seed, float* OctaveWeights, const int Octaves, const float min, const float max, const float Frequency);
	SimplexNoise* BaseNoise;
	int Octaves;
	float* OctaveWeights;
	float OutputPlus;//the addition to the output to map it between min and max
	float Frequency;//the multiplier for the input coordinates
	float* Evaluate(int X, int Y, int w, int l);
	float Evaluate(int aX, int aY);
	~LayerNoiseSimplex()
	{
		delete BaseNoise;
		delete[] OctaveWeights;
	}
};