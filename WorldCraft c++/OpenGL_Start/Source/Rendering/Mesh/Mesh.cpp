#include "Mesh.h"
#include "../Renderwindow.cpp"

Mesh::Mesh()
{
	GenVerticeBuffers(&VerticePointer, &VAO);
}

Mesh::~Mesh()
{
}
void Mesh::Draw(int PlaneCount)
{
	glBindVertexArray(VAO);//tell gl to use this buffer for further equations
	glDrawElements(GL_TRIANGLES, PlaneCount * PlaneTriangleCount * TriangleVerticeCount, GL_UNSIGNED_INT, nullptr);
}
void Mesh::SetMesh(float* vertices, int Size)
{
	if (Size <= 0) {
		enabled = false;
	}
	else
	{
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VerticePointer); //tell gl to use this buffer for further equations
		glBufferData(GL_ARRAY_BUFFER, Size * sizeof(float), vertices, GL_DYNAMIC_DRAW);
	}
}
void SolidParameters() {
	const int Stride3D = 3 + 2 + 3, FloatStride3D = Stride3D * sizeof(float),
		Vec2Size = 4,
		Vec3Size = 6,
		Vec3Stride = Vec3Size * sizeof(float),
		Vec2Stride = Vec2Size * sizeof(float);
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, FloatStride3D, (void*)0);
	glEnableVertexAttribArray(0);
	//texture attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, FloatStride3D, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// color attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, FloatStride3D, (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);
	//glDisableVertexAttribArray(0);
}
void FluidParameters() {
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (3 + 3) * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (3 + 3) * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}
void GenVerticeBuffers(GLuint * VerticeArray, GLuint * VAO) {
	glGenVertexArrays(1, VAO);
	glGenBuffers(1, VerticeArray);
	glBindVertexArray(*VAO);
	glBindBuffer(GL_ARRAY_BUFFER, *VerticeArray);
	//glbufferdata?
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
}