#include "LayerNoiseSimplex.h"
constexpr float MAX_NOISE_OUTPUT = 1;
constexpr float NOISE_MULT = 1.0f / MAX_NOISE_OUTPUT;//multiply output by this to map from -1 to 1
LayerNoiseSimplex::LayerNoiseSimplex(GLuint seed, float* OctaveWeights, const int Octaves, float min, float max, float Frequency)
{
	float TotalWeight = 0;
	BaseNoise = new SimplexNoise(seed);//initialize base noise
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

float* LayerNoiseSimplex::Evaluate(int X, int Y, const int w, const int l)
{
	const int size = w * l;
	float* OutputValues = new float[size];
	float* ptr = OutputValues;
	int EndX = X + w;
	int EndY = Y + l;
	for (int aY = Y; aY < EndY; aY++)
	{
		for (int aX = X; aX < EndX; aX++)
		{
			float val = OutputPlus;
			float freq = Frequency;
			for (int i = 0; i < Octaves; i++)
			{
				val += OctaveWeights[i] * BaseNoise->noise2(aX * freq, aY * freq);
				freq *= 2.0f;
			}
			*ptr++ = val;
		}
	}
	return OutputValues;
}
float LayerNoiseSimplex::Evaluate(int aX, int aY)
{
	float val = OutputPlus;
	float freq = Frequency;
	for (int i = 0; i < Octaves; i++)
	{
		val += OctaveWeights[i] * BaseNoise->noise2(aX * freq, aY * freq );
		freq *= 2.0f;
	}
	return val;
}