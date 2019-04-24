#include "Mob.h"
#pragma once
class Pig : public Mob
{
public:
	Pig(glm::vec3 position);
	void Render(ArrayMerger* merger);
};