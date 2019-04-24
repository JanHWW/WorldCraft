//typedef unsigned short ushort;
#include "../Functions/LightMap.h"
#include "../Rendering/ShaderProgram/Shader_m.h"
#include "../Texture/Texture.h"
//#include "../Noise/PerlinNoise.h"
//#include "../Noise/FastNoiseSIMD/FastNoiseSIMD.h"
//#include "../Noise/PerlinNoise.h"
#include "../Noise/SimplexNoise/LayerNoiseSimplex.h"
//#include "../Noise/RealNoise/LayerNoise.h"
#include "../Functions/ArrayDrawer.h"
#include "WorldConstants.h"
#include "ActionContainer.h"
#include "../Rendering/Mesh/Mesh.h"
#pragma once

#define Pow(e,pow)\
float p = e;\
for(int i = 1; i<pow;i++)\
p*=e;\
e = p;

extern Shader* WorldShader;
extern Shader* WaterShader;
extern GLuint IBO;
extern int Seed, Power;
extern float MinE, MaxE, MinM, MaxM;

Biome GetBiome(int PosX, int PosY);
void InitializeTerrain();
float* GetHeightMap(int x, int y, int w, int h, const int power, Biome* biomes);
class Chunk
{
public:
	GLuint CulledSolidVerticePointer;
	GLuint UnCulledSolidVerticePointer;
	GLuint FluidVerticePointer;
	GLuint CulledSolidVAO;
	GLuint UnCulledSolidVAO;
	GLuint FluidVAO;

	GLsizei FluidSize;
	GLsizei CulledSolidSize;
	GLsizei UnCulledSolidSize;

	Block data[ChunkScale3];//an 3d array that stores all the blocks

	BrightnessValue LightMap[ChunkScale3];//an 3d array that stores all the light levels

	int xPos;//the x position of the chunk
	int yPos;//the y position of the chunk

	bool Populated;//wether the chunk is populated(trees, actions)
	bool Changed;//wether the chunk needs to recalculate mesh positions and light

#pragma region Temporarily variables
	Biome* BiomeMap;
	int* Positions;//markers to places where anything needs to be placed
	Artifact* Artifacts;//what needs to be placed
	int PositionCount;//marker count
#pragma endregion

	Chunk();
	Chunk(int OffsetX, int OffsetY);

	void LoadData(); 
	void GenerateData();
	void GenerateMesh();
	void Populate();
	void DrawUnCulled() const;
	void DrawCulled() const;
	void DrawFluid() const;
	void PlaceOakTree(int x, int y, int z, int TrunkHeight);
	void PlacePalmTree(int x, int y, int z, int TrunkHeight);
	void PlacePineTree(int x, int y, int z, int TrunkHeight);
	void PlaceBirchTree(int x, int y, int z, int TrunkHeight);
	//called on deletion
	~Chunk();
private:
	void CalculateLightMap();
	void GeneratePlaneXY1(int x1, int y1, int z,LightIntensity intensity, float textureX, float textureY, float*& fptr);
	void GeneratePlaneXY0(int x1, int y1, int z, LightIntensity intensity, float textureX, float textureY, float*& fptr);
	void GeneratePlaneXZ0(int x1, int y, int z1, LightIntensity intensity, float textureX, float textureY, float*& fptr);
	void GeneratePlaneXZ1(int x1, int y, int z1, LightIntensity intensity, float textureX, float textureY, float*& fptr);
	void GeneratePlaneYZ0(int x, int y1, int z1, LightIntensity intensity, float textureX, float textureY, float*& fptr);
	void GeneratePlaneYZ1(int x, int y1, int z1, LightIntensity intensity, float textureX, float textureY, float*& fptr);
	void GenerateFluidPlaneXY1(int x1, int y1, int z, LightIntensity intensity, float*& fptr);
	void GenerateFluidPlaneXY0(int x1, int y1, int z, LightIntensity intensity, float*& fptr);
	void GenerateFluidPlaneXZ0(int x1, int y, int z1, LightIntensity intensity, float*& fptr);
	void GenerateFluidPlaneXZ1(int x1, int y, int z1, LightIntensity intensity, float*& fptr);
	void GenerateFluidPlaneYZ0(int x, int y1, int z1, LightIntensity intensity, float*& fptr);
	void GenerateFluidPlaneYZ1(int x, int y1, int z1, LightIntensity intensity, float*& fptr);
	void GenerateCross(int x1, int y1, int z1, LightIntensity intensity, float textureX, float textureY, float*& fptr);
};
#define FNV_32_PRIME 16777619u
unsigned int FNVHash32(const int input1, const int input2);
void SolidParameters();
void FluidParameters();
void GenVerticeBuffers(GLuint* VerticeArray, GLuint* VAO);