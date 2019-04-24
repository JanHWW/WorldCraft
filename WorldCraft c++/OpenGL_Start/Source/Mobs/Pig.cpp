#include "Pig.h"
//sources::
//https://minecraft.gamepedia.com/Pig
//https://www.tynker.com/minecraft/editor/mob/pig
constexpr float 
PigR = 1, PigG = 0.4f, PigB = 0.5f,
AdultPigHeight = 0.9f;
const int RectCount = 2;
Pig::Pig(glm::vec3 position)
{
	Position = position;
	//RectAngle rect = RectAngle(glm::mat3(1), Position, glm::vec3(1, 2, 1), glm::vec2(511.0f / 512.0f), glm::vec2(1));
	//rect.ptr = vertices;
	//rect.GenerateMesh();
}
void Pig::Render(ArrayMerger* merger)
{
	int VerticeCount = CubeStride3D * RectCount;
	float* vertices = new float[VerticeCount];
	Position.y += 0.01f;
	Position.x += rand() / (float)RAND_MAX - .5f;
	RectAngle body = RectAngle(glm::mat3(1), Position, glm::vec3(-.5f, -1, -.5f), glm::vec3(.5f, 1, .5f), glm::vec2(511.0f / 512.0f), glm::vec2(1), vertices);
	float headsize = .4f;
	RectAngle head = RectAngle(glm::mat3(1), glm::vec3( Position.x,Position.y + 1,Position.z + .4f), glm::vec3(-headsize), glm::vec3(headsize), glm::vec2(511.0f / 512.0f), glm::vec2(1), vertices + CubeStride3D);
	body.GenerateMesh();
	head.GenerateMesh();
	merger->AddArray(vertices, VerticeCount);
	//mesh = Mesh();
}