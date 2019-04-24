#pragma once
constexpr int MaxMem = 0x10000;//33 million
class  ArrayMerger
{
public:
	float MergedArray[MaxMem];
	int Size;
	void AddArray(float* ExtensiveArray, int count);
private:

};