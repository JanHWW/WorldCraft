#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aLight;

uniform vec3 EyePos;
uniform mat4 view;
uniform mat4 projection;
uniform float MaxDistance;

out vec3 Light;
out float Distance;

void main()
{
	gl_Position = projection * view * vec4(aPos, 1.0f);
	vec3 between = aPos - EyePos;
	Distance = dot(between,between)/MaxDistance;
	Light = aLight;
}