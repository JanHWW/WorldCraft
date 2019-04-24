#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Light;
in float Distance;
// texture samplers
uniform vec3 FogColor;
uniform sampler2D TextureContainer;

void main()
{
	if(Distance>1)discard;
	vec4 TexColor = texture(TextureContainer, TexCoord);
	if(TexColor.a<0.5)discard;//allow transparency
	vec3 WithoutFog = vec3(TexColor.r * Light.r,TexColor.g * Light.g,TexColor.b * Light.b);
	FragColor = vec4(mix(WithoutFog,FogColor,Distance),1f);//rgba

}