#version 330 core
//input parameters: positions, texture coordinates
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
out vec2 TexCoord;

void main()
{
	gl_Position = vec4(aPos.x*2-1,aPos.y*2-1,0.0f,1.0f);//range 0 to 1
	TexCoord = aTexCoord;
}