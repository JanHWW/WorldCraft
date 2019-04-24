#include "ArrayDrawer.h"
#include <iostream>
#include <stdio.h>
void ArrayMerger::AddArray(float* ExtensiveArray, int count) {
	if (Size + count >= MaxMem)
		throw new _exception();
	float* Mergedptr = MergedArray + Size;
	memcpy(Mergedptr, ExtensiveArray, count * sizeof(float));
	//copy
	//for (int i = 0; i < count; i++) {
	//	*Mergedptr++ = *ExtensiveArray++;
	//}
	Size += count;
}