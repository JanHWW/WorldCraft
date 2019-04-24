#include "Mob.h"
#include "../Rendering/RenderWindow.h"
void Mob::Render(ArrayMerger* merger)
{
	//merger->AddArray(vertices, VerticeCount);
}
void AddPoint(float* &ptr, glm::vec3 position, glm::vec2 Texture)
{
	*ptr++ = position.x;
	*ptr++ = position.y;
	*ptr++ = position.z;
	*ptr++ = Texture.x;
	*ptr++ = Texture.y;
	glm::vec3 light = GetLightLevel(floor(position.x), floor(position.y), floor(position.z));
	*ptr++ = light.r;
	*ptr++ = light.g;
	*ptr++ = light.b;
}
void RectAngle::GenerateMesh()
{
	float* ptr = this->ptr;
	float bx = Position.x + bp.x;
	float by = Position.y + bp.y;
	float bz = Position.z + bp.z;
	float ex = Position.x + ep.x;
	float ey = Position.y + ep.y;
	float ez = Position.z + ep.z;
	//bottom XY
	AddPoint(ptr, glm::vec3(bx, by, bz), indiceBegin);//0
	AddPoint(ptr, glm::vec3(ex, by, bz), glm::vec2(indiceEnd.x, indiceBegin.y));//1
	AddPoint(ptr, glm::vec3(bx, ey, bz), glm::vec2(indiceBegin.x, indiceEnd.y));//2
	AddPoint(ptr, glm::vec3(ex, ey, bz), indiceEnd);//3
	//top XY
	AddPoint(ptr, glm::vec3(bx, by, ez), indiceBegin);//0
	AddPoint(ptr, glm::vec3(ex, by, ez), glm::vec2(indiceEnd.x, indiceBegin.y));//1
	AddPoint(ptr, glm::vec3(bx, ey, ez), glm::vec2(indiceBegin.x, indiceEnd.y));//2
	AddPoint(ptr, glm::vec3(ex, ey, ez), indiceEnd);//3
	//forward XZ
	AddPoint(ptr, glm::vec3(bx, by, bz), indiceBegin);//0
	AddPoint(ptr, glm::vec3(ex, by, bz), glm::vec2(indiceEnd.x, indiceBegin.y));//1
	AddPoint(ptr, glm::vec3(bx, by, ez), glm::vec2(indiceBegin.x, indiceEnd.y));//2
	AddPoint(ptr, glm::vec3(ex, by, ez), indiceEnd);//3
	//backward XZ
	AddPoint(ptr, glm::vec3(bx, ey, bz), indiceBegin);//0
	AddPoint(ptr, glm::vec3(ex, ey, bz), glm::vec2(indiceEnd.x, indiceBegin.y));//1
	AddPoint(ptr, glm::vec3(bx, ey, ez), glm::vec2(indiceBegin.x, indiceEnd.y));//2
	AddPoint(ptr, glm::vec3(ex, ey, ez), indiceEnd);//3
	//left YZ
	AddPoint(ptr, glm::vec3(bx, by, bz), indiceBegin);//0
	AddPoint(ptr, glm::vec3(bx, ey, bz), glm::vec2(indiceEnd.x, indiceBegin.y));//1
	AddPoint(ptr, glm::vec3(bx, by, ez), glm::vec2(indiceBegin.x, indiceEnd.y));//2
	AddPoint(ptr, glm::vec3(bx, ey, ez), indiceEnd);//3
	//right YZ
	AddPoint(ptr, glm::vec3(ex, by, bz), indiceBegin);//0
	AddPoint(ptr, glm::vec3(ex, ey, bz), glm::vec2(indiceEnd.x, indiceBegin.y));//1
	AddPoint(ptr, glm::vec3(ex, by, ez), glm::vec2(indiceBegin.x, indiceEnd.y));//2
	AddPoint(ptr, glm::vec3(ex, ey, ez), indiceEnd);//3
}
RectAngle::RectAngle(glm::mat3 Transform, glm::vec3 Position, glm::vec3 bp, glm::vec3 ep, glm::vec2 indiceBegin, glm::vec2 indiceEnd, float* ptr)
{
	this->Transform = Transform;
	this->Position = Position;
	this->bp = bp;
	this->ep = ep;
	this->indiceBegin = indiceBegin;
	this->indiceEnd = indiceEnd;
	this->ptr = ptr;
}

RectAngle::~RectAngle()
{
}