#pragma once
#include "MeshConstants.h"
#include "../includeGL.h"
class Mesh {
public:
	Mesh();
	~Mesh();
	void Draw(int PlaneCount);
	void SetMesh(float* vertices, int Count);
	bool enabled;
private:
	GLuint VAO, VerticePointer;
};
void SolidParameters();
void FluidParameters();
void GenVerticeBuffers(GLuint* VerticeArray, GLuint* VAO);