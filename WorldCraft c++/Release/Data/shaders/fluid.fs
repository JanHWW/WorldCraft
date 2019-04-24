#version 330 core

in vec3 Light;
in float Distance;

uniform vec3 FogColor;
uniform vec4 FluidColor;

out vec4 FragColor;

void main()
{
	if(Distance>1)discard;
	vec3 WithoutFog = vec3(FluidColor.r * Light.r,FluidColor.g * Light.g,FluidColor.b * Light.b);
	FragColor = vec4(mix(WithoutFog,FogColor,Distance),FluidColor.a);//rgba
}