#include "../Rendering/Mesh/Mesh.h"
#pragma once
using namespace glm;
class Entity {
public:
	Mesh mesh;
	vec3 Position;
	vec3 Speed;
};