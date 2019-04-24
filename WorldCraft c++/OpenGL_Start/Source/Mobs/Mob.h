#include "../Functions/ArrayDrawer.h"
#include "../Entities/Entity.h"
#pragma once
//the base class of all mobs.
class Mob:public Entity
{
public:
	virtual void Render(ArrayMerger* merger);
private://things only available in this interface
protected://things that are available in the interface and its derivations
};
class RectAngle
{
public:
	float* ptr;
	glm::mat3 Transform;
	glm::vec3 Position;
	glm::vec3 bp, ep;//beginpoint and endpoint for the rectangle
	glm::vec2 indiceBegin;
	glm::vec2 indiceEnd;
	RectAngle(glm::mat3 Transform, glm::vec3 Position, glm::vec3 bp, glm::vec3 ep, glm::vec2 indiceBegin, glm::vec2 indiceEnd, float* ptr);
	~RectAngle();
	void GenerateMesh();
private:

};
void AddPoint(float* &ptr, glm::vec3 position, glm::vec2 Texture);