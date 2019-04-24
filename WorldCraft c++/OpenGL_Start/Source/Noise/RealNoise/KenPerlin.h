#pragma once
//source:
//https://www.mrl.nyu.edu/~perlin/doc/oscar.html#noise

#define B 0x100


struct Noise {
public:
	Noise(int seed);
	float noise3(float vec[3]);
	float noise2(float vec[2]);
	double noise1(double arg);
private:
	int p[B + B + 2];
	float g3[B + B + 2][3];
	float g2[B + B + 2][2];
	float g1[B + B + 2];
};