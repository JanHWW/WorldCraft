#include "../Texture/Texture.h"
#include "../Source/Functions/ArrayDrawer.h"
#include"../Functions/ArrayDrawer.h"
#include "../Source/Rendering/ShaderProgram/Shader_m.h"
#pragma once
void InitializeGUI();
void DeleteGUI();
void DrawAllControls();
class Control
{
public:
	Control(float Left, float Top, float Width, float Height, GLuint texture, float texX1 = 0, float texX2 = 1, float texY1 = 0, float texY2 = 1);
	void Draw(ArrayMerger* merger);
	void SetTextureCoords(float texX1 = 0, float texY1 = 0, float texX2 = 1, float texY2 = 1);
	~Control();
	unsigned int texture;
private:
	float vertices[6 * (2 + 2)];//two triangles
};