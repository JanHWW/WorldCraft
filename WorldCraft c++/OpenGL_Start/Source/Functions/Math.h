#pragma once
int Math_Fix(float f) {
	int i = (int)f;
	return i < f ? i++ : i;
}