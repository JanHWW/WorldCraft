#pragma once
#include "../Rendering/includeGL.h"
#define StoreFormat GL_RGBA
struct Texture {
public:
	GLubyte* data;
	int Width, Height, ChannelCount;
 	GLuint Format, Location;
	Texture(const char* path);
	Texture(int Width, int Height, int ChannelCount, GLubyte* data);
	GLubyte* GetColor(int x, int y);
	~Texture();
private:
	void Initialize();
};