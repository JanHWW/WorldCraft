#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
// texture samplers
uniform sampler2D TexContainer;

void main()
{
	vec4 color = texture(TexContainer, TexCoord);//don't forget the ;'s!
	if(color.a < 1)discard;
	FragColor = vec4(color.r,color.g,color.b,1);//color;
}