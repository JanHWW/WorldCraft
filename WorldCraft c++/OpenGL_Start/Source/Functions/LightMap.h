#pragma once
struct LightIntensity
{
public:
	float 
	b000,//b at x 0 y 0 z 0
	b100,//b at x 1 y 0 z 0
	b010,//b at x 0 y 1 z 0
	b110,//b at x 1 y 1 z 0
	b001,//b at x 0 y 0 z 1
	b101,//b at x 1 y 0 z 1
	b011,//b at x 0 y 1 z 1
	b111;//b at x 1 y 1 z 1
	void Shift();
};