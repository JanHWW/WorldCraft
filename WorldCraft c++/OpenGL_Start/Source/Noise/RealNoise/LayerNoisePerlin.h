#pragma once
#include "KenPerlin.h"//base noise
#include <glad/glad.h>//typedefs
class OptionNoise
{
public:
	//output range confirmed
	OptionNoise(GLuint seed, float* OctaveWeights, const int Octaves, const float min, const float max, const float Frequency);
	Noise* BaseNoise;
	int Octaves;
	float* OctaveWeights;
	float OutputPlus;//the addition to the output to map it between min and max
	float Frequency;//the multiplier for the input coordinates
	float* Evaluate(int X, int Y, int w, int l);
	float Evaluate(int aX, int aY);
	~OptionNoise()
	{
		delete BaseNoise;
		delete[] OctaveWeights;
	}
};