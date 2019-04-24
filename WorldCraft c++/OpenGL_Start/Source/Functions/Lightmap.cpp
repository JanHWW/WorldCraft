#include "LightMap.h"
#include <iostream>
#include <stdio.h>
void LightIntensity::Shift() {
	std::copy(&b000, &b000 + 12, &b001);
}
