//#include <GLFW/glfw3.h>
//#include <glm/glm.hpp>
#include "wtypes.h"
#include <iostream>
#include <stdio.h>
#include <list>
#include "ShaderProgram/Shader_m.h"
#include "../Texture/Texture.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../Functions/Functions.h"
#include <thread>
#include "../Chunk/Chunk.h"
#include "../Controls/Control.h"
#include "../Mobs/Pig.h"
#include "Math/Math.h"
#include "../Image/Image.h"
//#include <FreeImage/FreeImage.h>
using namespace std;
#pragma region Defines
#define JUMP_TO_BUILDPOS_KEY GLFW_KEY_K
#define GRAVITY_KEY GLFW_KEY_G
#define EXIT_KEY GLFW_KEY_ESCAPE
#define SNEAK_KEY GLFW_KEY_LEFT_SHIFT
#define SPRINT_KEY GLFW_KEY_LEFT_CONTROL
#define JUMP_KEY GLFW_KEY_SPACE
#define LEFT_KEY GLFW_KEY_A//GLFW_KEY_LEFT
#define RIGHT_KEY GLFW_KEY_D//GLFW_KEY_RIGHT
#define FORWARD_KEY GLFW_KEY_W//GLFW_KEY_UP
#define BACKWARD_KEY GLFW_KEY_S//GLFW_KEY_DOWN
#define BRAKE_KEY GLFW_KEY_R
#define LOOK_LEFT_KEY GLFW_KEY_LEFT//GLFW_KEY_A
#define LOOK_RIGHT_KEY GLFW_KEY_RIGHT//GLFW_KEY_D
#define LOOK_UP_KEY GLFW_KEY_UP//GLFW_KEY_W
#define LOOK_DOWN_KEY GLFW_KEY_DOWN//GLFW_KEY_S
#define PLACE_KEY GLFW_KEY_V
#define DIG_KEY GLFW_KEY_B
#define DROP_KEY GLFW_KEY_Q
#define INVENTORY_KEY GLFW_KEY_E
#define DigButton GLFW_MOUSE_BUTTON_LEFT
#define PlaceButton GLFW_MOUSE_BUTTON_RIGHT
#define GUI_KEY GLFW_KEY_N
#define SCREENSHOT_KEY GLFW_KEY_H
//https://minecraft.gamepedia.com/Player
#define FPS 30//60
#define MSFrame 1000/FPS
#define SneakingSpeed 1.31f //1.31 blocks per second
#define WalkingSpeed 4.317f //4.317 blocks per second
#define SprintingSpeed 5.612f //5.612 blocks per second
#define FlyingSpeed 10.92f //10.92 blocks per second
#define SprintFlyingSpeed 21.78f //21.78 blocks per second
#define MouseDown 1
#define PlayerWidth 0.6f // player is 0.6 blocks wide
#define JumpHeight 1.25219f//maximum height player can jump
#pragma endregion
#pragma once
constexpr float PI = 3.14159265359f;
constexpr float TickTime = 0.05f;
#pragma region Globals
#pragma region GL_Callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void click_callback(GLFWwindow* window, int button, int PressOrRelease, int Modifiers);
#pragma endregion
void GetDesktopResolution(int& horizontal, int& vertical);
#pragma endregion
#pragma region variables
//extern char* ScreenShotPath;
extern float MouseSensitivity, BackGroundR, BackGroundG, BackGroundB, WaterR, WaterG, WaterB;
extern int DesktopWidth, DesktopHeight;

extern GLFWwindow* OutputWindow;

#pragma endregion
	void SetActivePath(char* path);
	bool Run();
	void Draw();
	void processInput();
	void Physics();
	void Dig();
	void Place();
	void PopulateNearestNullChunk();
	void Documentate();
	void RemoveOutBounds();
	void ReloadTerrain();
	void Render3D();
	void GenerateControls();
	void Delete();
	void DeleteChunks();
	void Save(const char* path);
	void Open(const char* path);
	Block GetBlock(glm::vec3 pos);
	bool CreateRenderWindow(int Width, int Height, const char* Title);
	Block GetBlock(int x, int y, int z);
	glm::vec3 GetLightLevel(int x, int y, int z);
	void SetBlock(int x, int y, int z, Block value, const bool addaction);
	//void walk_grid(glm::vec3 p0, glm::vec3 p1);
	bool walk_grid(glm::vec3 p0, glm::vec3 p1, int &selx, int &sely, int &selz, int &adjx, int &adjy, int &adjz, Block &block);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	bool CollideHitbox(glm::vec3 v);