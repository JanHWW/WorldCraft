#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aLight;

out vec2 TexCoord;
out vec3 Light;
out float Distance;

uniform vec3 EyePos;
uniform mat4 view;
uniform mat4 projection;
uniform float MaxDistance;

void main()
{
	gl_Position = projection * view * vec4(aPos, 1.0f);
	vec3 between = aPos - EyePos;
	Distance = dot(between,between)/MaxDistance;
	TexCoord = aTexCoord;
	Light = aLight;
}