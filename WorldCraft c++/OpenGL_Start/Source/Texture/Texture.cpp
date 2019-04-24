//#include <stb_image.h>
#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION//needs to be BEFORE #include "ImageLoader/ImageLoader.h" (ifndef)
#include "ImageLoader/ImageLoader.h"

//STBIDEF void stbi_image_free(void *retval_from_stbi_load);
Texture::Texture(const char* path) {
	stbi_set_flip_vertically_on_load(true);
	data = stbi_load(path, &Width, &Height, &ChannelCount, 0);
	Format = ChannelCount == 3 ? GL_RGB : GL_RGBA;
	Initialize();
}
Texture::Texture(int Width, int Height, int ChannelCount, GLubyte* data)
{
	this->Width = Width;
	this->Height = Height;
	this->Format = Format;
	this->data = data;
	this->ChannelCount = ChannelCount;
	if (ChannelCount == 3)Format = GL_RGB;
	else Format = GL_RGBA;
	Initialize();
}
void Texture::Initialize() {
	if (data == nullptr) {
		//std::cout << "error loading textures" << std::endl;
		return;
	}
	glGenTextures(1, &Location);
	glBindTexture(GL_TEXTURE_2D, Location);

	glTexImage2D(GL_TEXTURE_2D, 0, StoreFormat //GL_RGB 
		, Width, Height, 0, Format, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}
Texture::~Texture()
{
	glDeleteTextures(1, &Location);
	stbi_image_free(data);
}
//https://stackoverflow.com/questions/48235421/get-rgb-of-a-pixel-in-stb-image
GLubyte* Texture::GetColor(int x, int y)
{
	GLubyte* pixelOffset = data + (x + y * Width) * ChannelCount;
	return new GLubyte[4]
	{
		*pixelOffset++,//r
		*pixelOffset++,//g
		*pixelOffset++,//b
		ChannelCount >= 4 ? (GLubyte)*pixelOffset : (GLubyte)0xff//a
	};
}