#include "Control.h"
constexpr int MAX_CONTROLS = 0x100;
Control* glblControls[MAX_CONTROLS];
Shader* GuiShader;
int ControlCount = 0;
int TextureLocation;
GLuint VAO, VBO;//vertex array object
ArrayMerger* MeshMerger;
extern GLuint IBO;
void DrawAllControls()
{
	//glDisable(GL_DEPTH_TEST);
	//glClear(GL_COLOR_BUFFER_BIT);
	GuiShader->bind();//tell opengl to use guishader
	glDepthFunc(GL_ALWAYS);
	glBindVertexArray(VAO);//tell gl to use this buffer for further equations
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	bool Drawed[MAX_CONTROLS];
	memset(Drawed, false, ControlCount);
	for (int i = 0; i < ControlCount; i++) {
		if (!Drawed[i]) {
			//Drawed[i] = true;//not neccesary because it will not be checked
			Control* control = glblControls[i];
			GLuint TextureLoc = control->texture;
			glBindTexture(GL_TEXTURE_2D, TextureLoc);//binds to active texture
			//glUniform1ui(TextureLocation, TextureLoc);
			MeshMerger->Size = 0;
			control->Draw(MeshMerger);
			for (int j = i + 1; j < ControlCount; j++) 
			{
				if (!Drawed[j] && glblControls[j]->texture == TextureLoc)
				{
					Drawed[j] = true;
					glblControls[j]->Draw(MeshMerger);
				}
			}
			glBufferData(GL_ARRAY_BUFFER, MeshMerger->Size * sizeof(float), MeshMerger->MergedArray, GL_DYNAMIC_DRAW);
			//glDrawArrays(GL_TRIANGLES, 0, MeshMerger->Size);
			glDrawElements(GL_TRIANGLES, MeshMerger->Size / (4 * 4) * 6, GL_UNSIGNED_INT,  nullptr);
			glBindTexture(GL_TEXTURE_2D, 0);//unbinds to active texture
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	GuiShader->unbind();
}
void InitializeGUI()
{
	const int Stride2D = 2 + 2, FloatStride2D = Stride2D * sizeof(float);
	GuiShader = new Shader("data/shaders/gui.vs", "data/shaders/gui.fs");
	TextureLocation = glGetUniformLocation(GuiShader->ID, "TexContainer");
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	// position attribute
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, FloatStride2D, (void*)0);
	glEnableVertexAttribArray(0);
	//texture attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, FloatStride2D, (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	MeshMerger = new ArrayMerger();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);
}
void DeleteGUI() {
	delete GuiShader;
}
Control::Control(float Left, float Top, float Width, float Height, GLuint Texture, float texX1, float texX2, float texY1, float texY2)
{
	float x2 = Left + Width, y2 = Top + Height;
	float* ptr = vertices;
	//pos x y, tex x y
	*ptr++ = Left; *ptr++ = Top; *ptr++ = texX1; *ptr++ = texY1; //0 0 0
	*ptr++ = x2; *ptr++ = Top; *ptr++ = texX2; *ptr++ = texY1; //1 0 1
	*ptr++ = Left; *ptr++ = y2; *ptr++ = texX1; *ptr++ = texY2; //2 1 0
	*ptr++ = x2; *ptr++ = y2; *ptr++ = texX2; *ptr++ = texY2; //3 1 1
	this->texture = Texture;
	glblControls[ControlCount++] = this;
}
void Control::SetTextureCoords(float texX1, float texY1, float texX2, float texY2) {
	float* ptr = vertices + 2;
	*ptr++ = texX1; *ptr++ = texY1; //0 0 0
	ptr += 2;//jump over other indices
	*ptr++ = texX2; *ptr++ = texY1; //1 0 1
	ptr += 2;//jump over other indices
	*ptr++ = texX1; *ptr++ = texY2; //2 1 0
	ptr += 2;//jump over other indices
	*ptr++ = texX2; *ptr++ = texY2; //3 1 1
}

void Control::Draw(ArrayMerger* merger)
{
	merger->AddArray(vertices, 4 * 4);
}

Control::~Control()
{

}

