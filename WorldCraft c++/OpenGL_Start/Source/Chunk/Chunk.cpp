#include "Chunk.h"
#include "../Functions/Math.h"
#include <time.h>
using namespace std;
extern void SetBlock(int x, int y, int z, Block value, const bool addaction = false);
extern Block GetBlock(int x, int y, int z);
extern void SetBlockRange(int x0, int y0, int z0, int x1, int y1, int z1, Block value, const bool addaction = false);
LayerNoiseSimplex* HeightNoise;
LayerNoiseSimplex* MoistureNoise;
int Seed;
int Power;
float MinE, MaxE;
float MinM, MaxM;
inline Biome GetBiome(float e, float m) {
	return

		(e < 0.1) ? OCEAN :

		(e < 0.12) ? BEACH :

		(e > 0.8) ?
		(m < 0.1) ? SCORCHED :
		(m < 0.2) ? BARE :
		(m < 0.5) ? TUNDRA :
		SNOW :

		(e > 0.6) ?
		(m < 0.33) ? TEMPERATE_DESERT :
		(m < 0.66) ? SHRUBLAND :
		TAIGA :

		(e > 0.3) ?
		(m < 0.16) ? TEMPERATE_DESERT :
		(m < 0.50) ? GRASSLAND :
		(m < 0.83) ? TEMPERATE_DECIDUOUS_FOREST :
		TEMPERATE_RAIN_FOREST :

		(m < 0.16) ? SUBTROPICAL_DESERT :
		(m < 0.33) ? GRASSLAND :
		(m < 0.66) ? TROPICAL_SEASONAL_FOREST :
		TROPICAL_RAIN_FOREST;
}
void InitializeTerrain()
{
	int NoiseSeed = Seed;
	HeightNoise = new LayerNoiseSimplex(NoiseSeed, new float[Octaves] {1, .5f, .25f, .125f, .06125f, .036125f}, Octaves, MinE, MaxE, FREQUENCY);
	MoistureNoise = new LayerNoiseSimplex(NoiseSeed + 1, new float[BIOME_OCTAVES] {1, .75f, .33f, .33f, .33f, .45f}, BIOME_OCTAVES, MinM, MaxM, BIOME_FREQUENCY);
}
Biome GetBiome(int X, int Y)
{
	float elevation = HeightNoise->Evaluate(X, Y);
	Pow(elevation, Power);
	float moisture = MoistureNoise->Evaluate(X, Y);
	return GetBiome(elevation, moisture);
}

float* GetHeightMap(int x, int y, const int w, const int l, Biome* Biomes)
{
	const int size = w * l;
	float* HeightMap = new float[size];
	float* eValues = HeightNoise->Evaluate(x, y, w, l);
	float* mValues = MoistureNoise->Evaluate(x, y, w, l);
	float* mPtr = mValues;//pointer to moisture values
	float* eEndPtr = eValues + size;
	float* HeightMapPtr = HeightMap;
	for (float* ptr = eValues; ptr < eEndPtr; ptr++) {
		//pow(e,i)
		Pow(*ptr, Power);
		Biome biome = GetBiome(*ptr, *mPtr++);
			*Biomes++ = biome;
			//*Biomes++ = GetBiome(*ptr, *mPtr++);
		*HeightMapPtr++ = *ptr
			* GenDiff + MinGenHeight;
	}
	//delete[] mValues, eValues; does not give the expected behaviour.
	//sources:
	//https://stackoverflow.com/questions/6694745/how-to-delete-multiple-dynamically-allocated-arrays-in-a-single-delete-statement
	//http://stackoverflow.com/questions/3037655/c-delete-syntax
	delete[] mValues,delete[] eValues;
	return HeightMap;
}
//#include "../Noise/PerlinNoise.h"
//#include "../Noise/FastNoiseSIMD/FastNoiseSIMD.h"
unsigned int FNVHash32(const int input1, const int input2)
{
	unsigned int hash = 2166136261u;
	const unsigned char* pBuf = (unsigned char *)&input1;

	for (int i = 0; i < 4; ++i)
	{
		hash *= FNV_32_PRIME;
		hash ^= *pBuf++;
	}

	pBuf = (unsigned char *)&input2;

	for (int i = 0; i < 4; ++i)
	{
		hash *= FNV_32_PRIME;
		hash ^= *pBuf++;
	}

	return hash;
}
Chunk::Chunk() {}
Chunk::Chunk(int OffsetX, int OffsetY)
{
		this->xPos = OffsetX;
		this->yPos = OffsetY;
		this->Populated = false;
		//this->vertices = new float[0];

		GenVerticeBuffers(&CulledSolidVerticePointer, &CulledSolidVAO);
		SolidParameters();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		GenVerticeBuffers(&UnCulledSolidVerticePointer, &UnCulledSolidVAO);
		SolidParameters();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		GenVerticeBuffers(&FluidVerticePointer, &FluidVAO);
		FluidParameters();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		LoadData();
}
//loads data.
//if file exists, load from file, else generate
void Chunk::LoadData()
{
	GenerateData();
}
void Chunk::GenerateData() {
	Block* ptr = this->data;
	int ScaleOffsetX = xPos * ChunkScaleHor;
	int ScaleOffsetY = yPos * ChunkScaleHor;
	float NoiseScale = 10;
	const int MAX_ARTIFACTS = ChunkScale2;
	int Positions[MAX_ARTIFACTS * 3];
	Artifact Artifacts[MAX_ARTIFACTS];
	int PositionCount = 0;
	int difference = Maxheight - Bedrocklayers;
	Biome* BiomeMap = new Biome[ChunkScale2];
	float* HeightMap = GetHeightMap(ScaleOffsetX, ScaleOffsetY, ChunkScaleHor, ChunkScaleHor, BiomeMap);
	Biome* biomePtr = BiomeMap;

	//initialize chunk seed
	int seed = FNVHash32(xPos, yPos);
	srand(seed);

#pragma region DirtWaterGrassAndAir
	//fill region below or equal to minheight with bedrock
	for (int j = 0; j < ChunkScaleHor; j++) {
		for (int i = 0; i < ChunkScaleHor; i++) {
			//get map data
			int height = HeightMap[i + j * StrideY];
			Biome ActiveBiome = *biomePtr++;

			int r = rand() % 1000;//1000 possibilities
			const int* ChancePtr = chance + (int)ActiveBiome * ArtifactCount;

			//determine if artifact is placed and what type
			for (int artIndex = 0; artIndex < ArtifactCount; artIndex++)
			{
				if (r < ChancePtr[artIndex])
				{
					Artifacts[PositionCount / 3] = (Artifact)artIndex;
					Positions[PositionCount++] = i;
					Positions[PositionCount++] = j;
					Positions[PositionCount++] = height;
					break;//else all the other artifacts will also be placed
				}
			}

			for (int k = 0; k < ChunkScaleVert; k++)
			{
				Block* ActiveBlock = data + i + j * StrideY + k * StrideZ;
				if (k < height)
				{
					const int GroundLayers = 4;
					if (k < height - GroundLayers)
					{
						*ActiveBlock = Stone;
					}
					else 
					{
						*ActiveBlock = LayerBlock[ActiveBiome];
					}
				}
				else if (k == height) 
				{
					*ActiveBlock = TopBlock[ActiveBiome];
				}
				else {
					if (k < SeaLevel)
					{
						*ActiveBlock = Water;
					}
					else
					{
						*ActiveBlock = Air;
					}
				}
			}
		}
	}
	delete[] HeightMap;
	delete[] BiomeMap;
#pragma endregion
	//positions
	this->PositionCount = PositionCount;

	//copy artifact data
	int* posPtr = new int[PositionCount];
	memcpy(posPtr, Positions, PositionCount * sizeof(int));
	this->Positions = posPtr;
	//artifacts
	Artifact* artifactPtr = new Artifact[PositionCount / 3];
	memcpy(artifactPtr, Artifacts, PositionCount / 3 * sizeof(Artifact));
	this->Artifacts = artifactPtr;
#pragma endregion
}
Chunk::~Chunk()
{
	glDeleteVertexArrays(1, &CulledSolidVAO);
	glDeleteBuffers(1, &CulledSolidVerticePointer);
	glDeleteVertexArrays(1, &UnCulledSolidVAO);
	glDeleteBuffers(1, &UnCulledSolidVerticePointer);
	glDeleteVertexArrays(1, &FluidVAO);
	glDeleteBuffers(1, &FluidVerticePointer);
	//destroys object
}
//https://minecraft.gamepedia.com/Tree
//https://github.com/ferreusveritas/DynamicTrees/wiki/World-generation
void Chunk::PlaceBirchTree(int x, int y, int z, int TrunkHeight)
{
	int bottomz = z + TrunkHeight;
	int TrunkTopZ = bottomz + 3;
	//trunk
	SetBlockRange(x, y, z, x + 1, y + 1, TrunkTopZ, Block::BirchWood);

	//layer 1
	//-y
	SetBlock(x, y - 1, bottomz + 3, Leaves);
	//+y
	SetBlock(x, y + 1, bottomz + 3, Leaves);
	//x
	SetBlockRange(x - 1, y, bottomz + 3, x + 2, y + 1, bottomz + 4, Leaves);

	//layer 2
	//-y
	SetBlockRange(x - 1, y - 1, bottomz + 2, x + 2, y, bottomz + 3, Leaves);
	//+y
	SetBlockRange(x - 1, y + 1, bottomz + 2, x + 2, y + 2, bottomz + 3, Leaves);
	//-x
	SetBlock(x - 1, y, bottomz + 2, Leaves);
	//+x
	SetBlock(x + 1, y, bottomz + 2, Leaves);

	//layer 3
	//-x
	SetBlockRange(x - 2, y - 1, bottomz + 1, x, y + 2, bottomz + 2, Leaves);
	//+x
	SetBlockRange(x + 1, y - 1, bottomz + 1, x + 3, y + 2, bottomz + 2, Leaves);
	//-y
	SetBlockRange(x - 1, y - 2, bottomz + 1, x + 2, y - 1, bottomz + 2, Leaves);
	//fitting block
	SetBlock(x, y - 1, bottomz + 1, Leaves);
	//+y
	SetBlockRange(x - 1, y + 2, bottomz + 1, x + 2, y + 3, bottomz + 2, Leaves);
	//fitting blocks:
	//+y
	SetBlock(x, y + 1, bottomz + 2, Leaves);
	//-y
	SetBlock(x, y - 1, bottomz + 2, Leaves);

	//layer 4
	//-x
	SetBlockRange(x - 2, y - 2, bottomz, x, y + 3, bottomz + 1, Leaves);
	//+x
	SetBlockRange(x + 1, y - 2, bottomz, x + 3, y + 3, bottomz + 1, Leaves);
	//y
	SetBlock(x, y - 2, bottomz, Leaves);
	SetBlock(x, y - 1, bottomz, Leaves);
	SetBlock(x, y + 1, bottomz, Leaves);
	SetBlock(x, y + 2, bottomz, Leaves);
}
void Chunk::PlaceOakTree(int x, int y, int z, int TrunkHeight)
{
	int TrunkTopZ = z + TrunkHeight;
	SetBlockRange(x, y, z, x + 1, y + 1, TrunkTopZ, Block::OakWood);

	SetBlockRange(x - 2, y - 2, TrunkTopZ, x + 3, y + 3, TrunkTopZ + 3, Leaves);

	int LeafHeight = TrunkTopZ + 1;
	SetBlock(x - 3, y, LeafHeight, Leaves);
	SetBlock(x, y - 3, LeafHeight, Leaves);
	SetBlock(x + 3, y, LeafHeight, Leaves);
	SetBlock(x, y + 3, LeafHeight, Leaves);
}
void Chunk::PlacePalmTree(int x, int y, int z, int TrunkHeight)
{
	int TrunkTopZ = z + TrunkHeight;
	SetBlockRange(x, y, z, x + 1, y + 1, TrunkTopZ, Block::PalmWood);

	//cross
	SetBlock(x - 2, y, TrunkTopZ, Leaves);//x row
	SetBlock(x - 1, y, TrunkTopZ, Leaves);
	SetBlock(x + 1, y, TrunkTopZ, Leaves);
	SetBlock(x + 2, y, TrunkTopZ, Leaves);
	SetBlockRange(x, y - 2, TrunkTopZ, x + 1, y + 3, TrunkTopZ + 1, Leaves);//y row
	//top leaf
	SetBlock(x, y, TrunkTopZ + 1, Leaves);
	//hanging leaves
	int LeafHeight = TrunkTopZ - 1;
	SetBlock(x - 3, y, LeafHeight, Leaves);
	SetBlock(x, y - 3, LeafHeight, Leaves);
	SetBlock(x + 3, y, LeafHeight, Leaves);
	SetBlock(x, y + 3, LeafHeight, Leaves);
}
//best height: 2
void Chunk::PlacePineTree(int x, int y, int z, int TrunkHeight)
{
	int TrunkTopZ = z + TrunkHeight + 3;
	SetBlockRange(x, y, z, x + 1, y + 1, TrunkTopZ, Block::OakWood);

	//top cross
	SetBlock(x - 1, y, TrunkTopZ, Leaves);//x row
	SetBlock(x + 1, y, TrunkTopZ, Leaves);
	SetBlock(x, y - 1, TrunkTopZ, Leaves);//y row
	SetBlock(x, y, TrunkTopZ, Leaves);
	SetBlock(x, y + 1, TrunkTopZ, Leaves);
	//top leaf
	SetBlock(x, y, TrunkTopZ + 1, Leaves);
	//circles of leaves
	//small circle
	int SmallCircleHeight = TrunkTopZ - 2;
	SetBlock(x - 1, y, SmallCircleHeight, Leaves);//cross
	SetBlock(x, y - 1, SmallCircleHeight, Leaves);
	SetBlock(x + 1, y, SmallCircleHeight, Leaves);
	SetBlock(x, y + 1, SmallCircleHeight, Leaves);
	//big circle
	int BigCircleHeight = TrunkTopZ - 3;
	SetBlockRange(x - 2, y - 1, BigCircleHeight, x, y + 2, BigCircleHeight + 1, Leaves);//-x
	SetBlockRange(x + 1, y - 1, BigCircleHeight, x + 3, y + 2, BigCircleHeight + 1, Leaves);//+x
	SetBlockRange(x - 1, y - 2, BigCircleHeight, x + 2, y - 1, BigCircleHeight + 1, Leaves);//-y
	SetBlockRange(x - 1, y + 2, BigCircleHeight, x + 2, y + 3, BigCircleHeight + 1, Leaves);//+y
	SetBlock(x, y - 1, BigCircleHeight, Leaves);//fitting block: -y
	SetBlock(x, y + 1, BigCircleHeight, Leaves);//fitting block: +y
}
const int MAX_CULLED_SOLID_VERTICES = 0x100000;//a million
const int MAX_UNCULLED_SOLID_VERTICES = 0x100000;//a million
const int MAX_FLUID_VERTICES = 0x100000;//65 thousand
void Chunk::GenerateMesh() {
	float* CulledSolidVerticesContainer = new float[MAX_CULLED_SOLID_VERTICES];
	float* UnCulledSolidVerticesContainer = new float[MAX_UNCULLED_SOLID_VERTICES];
	float* FluidVerticesContainer = new float[MAX_FLUID_VERTICES];
	CalculateLightMap();
	//delete[] vertices;
	Changed = false;
	int CulledSolidPlaneCount = 0;
	int UnCulledSolidPlaneCount = 0;
	int FluidPlaneCount = 0;
	float* CulledSolidPtr = CulledSolidVerticesContainer;
	float* UnCulledSolidPtr = UnCulledSolidVerticesContainer;
	float* FluidPtr = FluidVerticesContainer;
	int ScaleOffsetX = xPos * ChunkScaleHor;
	int ScaleOffsetY = yPos * ChunkScaleHor;
	LightIntensity intensity = LightIntensity();
	for (int j = 0; j < ChunkScaleHor; j++) {
	//for (int j = 1; j < ChunkScaleHor - 1; j++) {
			int RealY = j + ScaleOffsetY;
		for (int i = 0; i < ChunkScaleHor; i++) {
		//for (int i = 1; i < ChunkScaleHor - 1; i++) {
				int RealX = i + ScaleOffsetX;
			BrightnessValue* lightptr = LightMap + i + j * StrideY + (ChunkScaleVert - 1) * StrideZ;
			for (int k = ChunkScaleVert - 1; k >= 0; k--) {
				int index = i + j * StrideY + k * StrideZ;
				Block* ptr = data + index;
				Block block = *ptr;
				//intensity.Shift();
				float top = *lightptr * BrightnessValueToFloat;
				intensity.b001 = top;
				intensity.b101 = top;
				intensity.b011 = top;
				intensity.b111 = top;
				if (k > 0) {
					lightptr -= StrideZ;//z--
					float bottom = *lightptr * BrightnessValueToFloat;
					intensity.b000 = bottom;
					intensity.b100 = bottom;
					intensity.b010 = bottom;
					intensity.b110 = bottom;
				}
				//std::copy(&intensity.b000, &intensity.b001, &intensity.b001);
				//if(block < MinSolid && block != Leaves)
				if (block > Air)
				{
					if (block == Water)
					{
						if (HasAir[GetBlock(RealX-1,RealY,k)])
						{
							GenerateFluidPlaneYZ0(RealX, RealY, k, intensity, FluidPtr);
							FluidPlaneCount ++;
						}
						if (HasAir[GetBlock(RealX, RealY-1, k)])
						{
							GenerateFluidPlaneXZ0(RealX, RealY, k, intensity, FluidPtr);
							FluidPlaneCount ++;
						}
						if (k > 0 && HasAir[GetBlock(RealX , RealY, k-1)])
						{
							GenerateFluidPlaneXY0(RealX, RealY, k, intensity, FluidPtr);
							FluidPlaneCount ++;
						}
						if (HasAir[GetBlock(RealX + 1, RealY, k)])
						{
							GenerateFluidPlaneYZ1(RealX + 1, RealY, k, intensity, FluidPtr);
							FluidPlaneCount ++;
						}
						if (HasAir[GetBlock(RealX , RealY+1, k)])
						{
							GenerateFluidPlaneXZ1(RealX, RealY + 1, k, intensity, FluidPtr);
							FluidPlaneCount ++;
						}
						if (HasAir[GetBlock(RealX, RealY, k + 1)])
						{
							GenerateFluidPlaneXY1(RealX, RealY, k + 1, intensity, FluidPtr);
							FluidPlaneCount ++;
						}
						continue;
					}
					float TextureSideX = textureIndices[block * TextIndiceCount];
					float TextureSideY = textureIndices[block * TextIndiceCount + 1];
					if (GetBlockType[block] == Cross) {//cross
						GenerateCross(RealX, RealY, k, intensity, TextureSideX, TextureSideY, UnCulledSolidPtr);
						UnCulledSolidPlaneCount += 2;//two planes
						continue;
					}
					float** activePtr;
					int* activePlaneCount;
					if (IsVisibleSurrounded[block]) {
						activePtr = &UnCulledSolidPtr;
						activePlaneCount = &UnCulledSolidPlaneCount;
					}
					else 
					{
						activePtr = &CulledSolidPtr;
						activePlaneCount = &CulledSolidPlaneCount;
					}
					if (IsVisibleSurrounded[GetBlock(RealX - 1, RealY, k)]) {//x--
						GeneratePlaneYZ0(RealX, RealY, k, intensity, TextureSideX, TextureSideY, *activePtr);
						(*activePlaneCount)++;
					}
					if (IsVisibleSurrounded[GetBlock(RealX, RealY - 1, k)]) {//y--
						GeneratePlaneXZ0(RealX, RealY, k, intensity, TextureSideX, TextureSideY, *activePtr);
						(*activePlaneCount)++;
					}
					if (k > 0 && IsVisibleSurrounded[GetBlock(RealX, RealY, k - 1)]) {//z--
						float TextureBottomX = textureIndices[block * TextIndiceCount + 4];
						float TextureBottomY = textureIndices[block * TextIndiceCount + 5];
						GeneratePlaneXY0(RealX, RealY, k, intensity, TextureBottomX, TextureBottomY, *activePtr);
						(*activePlaneCount)++;
					}
					float TextureTopX = textureIndices[block * TextIndiceCount + 2];
					float TextureTopY = textureIndices[block * TextIndiceCount + 3];
					if (DrawAllSides[block]) {
						if (IsVisibleSurrounded[GetBlock(RealX + 1, RealY, k)]) {//x++
							GeneratePlaneYZ1(RealX + 1, RealY, k, intensity, TextureSideX, TextureSideY, *activePtr);
							(*activePlaneCount)++;
						}
						if (IsVisibleSurrounded[GetBlock(RealX, RealY + 1, k)]) {//y++
							GeneratePlaneXZ1(RealX, RealY + 1, k, intensity, TextureSideX, TextureSideY, *activePtr);
							(*activePlaneCount)++;
						}
						if (IsVisibleSurrounded[GetBlock(RealX, RealY, k + 1)]) {//z++
							GeneratePlaneXY1(RealX, RealY, k + 1, intensity, TextureTopX, TextureTopY, *activePtr);
							(*activePlaneCount)++;
						}
					}
					else {//blocks with transparent parts in it
						//==air because leaves would be drawn twice every block
						if (!IsSolidBlock[GetBlock(RealX + 1, RealY, k)]) {//x++
							GeneratePlaneYZ1(RealX + 1, RealY, k, intensity, TextureSideX, TextureSideY, *activePtr);
							(*activePlaneCount)++;
						}
						if (!IsSolidBlock[GetBlock(RealX, RealY + 1, k)]) {//y++
							GeneratePlaneXZ1(RealX, RealY + 1, k, intensity, TextureSideX, TextureSideY, *activePtr);
							(*activePlaneCount)++;
						}
						if (!IsSolidBlock[GetBlock(RealX, RealY, k + 1)]) {//z++
							GeneratePlaneXY1(RealX, RealY, k + 1, intensity, TextureTopX, TextureTopY, *activePtr);
							(*activePlaneCount)++;
						}
					}
				}
			}
		}
	}
	//culled solid
	CulledSolidSize = CulledSolidPlaneCount * 6;
	glBindVertexArray(CulledSolidVAO);
	glBindBuffer(GL_ARRAY_BUFFER, CulledSolidVerticePointer); //tell gl to use this buffer for further equations
	glBufferData(GL_ARRAY_BUFFER, CulledSolidPlaneCount * ((3 + 2 + 3) * 4) * sizeof(float), CulledSolidVerticesContainer, GL_DYNAMIC_DRAW);
	//unculled solid
	UnCulledSolidSize = UnCulledSolidPlaneCount * 6;
	glBindVertexArray(UnCulledSolidVAO);
	glBindBuffer(GL_ARRAY_BUFFER, UnCulledSolidVerticePointer); //tell gl to use this buffer for further equations
	glBufferData(GL_ARRAY_BUFFER, UnCulledSolidPlaneCount* ((3 + 2 + 3) * 4) * sizeof(float), UnCulledSolidVerticesContainer, GL_DYNAMIC_DRAW);
	//fluid
	FluidSize = FluidPlaneCount * 6;
	glBindVertexArray(FluidVAO);
	glBindBuffer(GL_ARRAY_BUFFER, FluidVerticePointer); //tell gl to use this buffer for further equations
	glBufferData(GL_ARRAY_BUFFER, FluidPlaneCount * ((3 + 3) * 4) * sizeof(float), FluidVerticesContainer, GL_DYNAMIC_DRAW);
	//delete unneccesary stuff
	delete[] FluidVerticesContainer;
	delete[] CulledSolidVerticesContainer;
	delete[] UnCulledSolidVerticesContainer;
}
void Chunk::Populate() 
{
	//initialize chunk seed
	int seed = FNVHash32(xPos, yPos);
	srand(seed);

	//load structures
#pragma region TreesAndFlowers
	Block* ptr = this->data;
	int ScaleOffsetX = xPos * ChunkScaleHor;
	int ScaleOffsetY = yPos * ChunkScaleHor;

	for (int i = 0; i < PositionCount;)
	{
		Artifact artifact = Artifacts[i / 3];
		int TreeI = Positions[i++];
		int TreeJ = Positions[i++];
		int TreeK = Positions[i++] + 1;//origin 1 higher
		BlockIndex index = TreeI + TreeJ * StrideY + TreeK * StrideZ;
		int TreeX = TreeI + ScaleOffsetX;
		int TreeY = TreeJ + ScaleOffsetY;
		switch (artifact)
		{
			case aOakTree:
				PlaceOakTree(TreeX, TreeY, TreeK, rand() % 3 + 3);
				continue;
			case aBirchTree:
				PlaceBirchTree(TreeX, TreeY, TreeK, rand() % 3 + 2);//5 to seven blocks tall
				continue;
			case aPalmTree:
				PlacePalmTree(TreeX, TreeY, TreeK, rand() % 3 + 3);
				continue;
			case aPineTree:
				PlacePineTree(TreeX, TreeY, TreeK, rand() % 3 + 1);
				continue;
			case aBlueFlower:
				SetBlock(TreeX, TreeY, TreeK, Block::BlueFlower);
				continue;
			case aRedFlower:
				SetBlock(TreeX, TreeY, TreeK, Block::RedFlower);
				continue;
			case aOrangeFlower:
				SetBlock(TreeX, TreeY, TreeK, Block::OrangeFlower);
				continue;
			default:
				continue;
			}
	placed:;
	}
	ActionContainer* container = GetContainer(xPos, yPos);
	if (container != NULL) {
		std::list<Action*>* actions = container->actions;
		for (auto ActiveAction = actions->begin(); ActiveAction != actions->end(); ActiveAction++)
		{
			Action a = **ActiveAction;
			*(data + a.index) = a.block;
		}
	}
	delete[] Positions;
	delete[] Artifacts;
	Changed = true;
	Populated = true;
}
//https://stackoverflow.com/questions/4535133/how-does-minecraft-perform-lighting
void Chunk::CalculateLightMap() {
	Block* bBegin = data + (ChunkScaleVert - 1) * StrideZ;//z plus
	//the lowest block to check: 1, because there is nothing underneath 0
	float AboveBlockOpacity;
	BrightnessValue MinBrightness = 0x80;
	BrightnessValue SunLight = 0xff;
	for (int j = 0; j < ChunkScaleHor; j++) {//y++
		Block* jBlockPlus = bBegin + j * StrideY;
		BrightnessValue* YPlus = LightMap + j * StrideY;
		for (int i = 0; i < ChunkScaleHor; i++) {//x++
			BrightnessValue* bvLowest = YPlus + i;
			BrightnessValue* bvActive = bvLowest + (ChunkScaleVert - 1) * StrideZ;
			//bv: csv - 1 to 0
			*bvActive = SunLight;//set all upper values to sunlight
			bvActive -= StrideZ;
			//blockabove: csv -1 to 1
			Block* AboveBlock = jBlockPlus + i;
			while (bvActive >= bvLowest)
			{
				AboveBlockOpacity = Opacity[*AboveBlock];//x y
				*bvActive = AboveBlockOpacity * *(bvActive + StrideZ);
				if (*bvActive < MinBrightness) {
					while (bvActive >= bvLowest) {//fill the rest with minimum
						*bvActive = MinBrightness;
						bvActive -= StrideZ;//z--
					}
					goto next;
				}
				bvActive -= StrideZ;
				AboveBlock -= StrideZ;
			}
		next:;
		}
	}
}
void Chunk::DrawCulled() const//const means that they do not change the base class
{
	if (CulledSolidSize > 0) 
	{
		glBindVertexArray(CulledSolidVAO);//tell gl to use this buffer for further equations
		glDrawElements(GL_TRIANGLES, CulledSolidSize, GL_UNSIGNED_INT, nullptr);
	}
}
void Chunk::DrawUnCulled() const
{
	if (UnCulledSolidSize > 0)
	{
		glBindVertexArray(UnCulledSolidVAO);//tell gl to use this buffer for further equations
		glDrawElements(GL_TRIANGLES, UnCulledSolidSize, GL_UNSIGNED_INT, nullptr);
	}
}
void Chunk::DrawFluid() const
{
	if (FluidSize > 0)
	{
		glBindVertexArray(FluidVAO);//tell gl to use this buffer for further equations
		glDrawElements(GL_TRIANGLES, FluidSize, GL_UNSIGNED_INT, nullptr);
	}
}
//[axis] 0 is inverted to let the normals point to the outside
void Chunk::GeneratePlaneXY0(int x1, int y1, int z, LightIntensity intensity, float textureX, float textureY, float*& fptr) {
	float textureX1 = textureX + TextSize;
	float textureY1 = textureY + TextSize;
	int x2 = x1 + 1;
	int y2 = y1 + 1;
	*fptr++ = x1;	*fptr++ = y1;	*fptr++ = z; *fptr++ = textureX; *fptr++ = textureY; *fptr++ = intensity.b000; *fptr++ = intensity.b000; *fptr++ = intensity.b000;//000,00,0
	*fptr++ = x1;	*fptr++ = y2;	*fptr++ = z; *fptr++ = textureX; *fptr++ = textureY1; *fptr++ = intensity.b010; *fptr++ = intensity.b010; *fptr++ = intensity.b010;//010,01,2
	*fptr++ = x2;	*fptr++ = y1;	*fptr++ = z; *fptr++ = textureX1; *fptr++ = textureY; *fptr++ = intensity.b100; *fptr++ = intensity.b100; *fptr++ = intensity.b100;//100,10,1
	*fptr++ = x2;	*fptr++ = y2;	*fptr++ = z; *fptr++ = textureX1; *fptr++ = textureY1; *fptr++ = intensity.b110; *fptr++ = intensity.b110; *fptr++ = intensity.b110;//110,11,3
}
void Chunk::GeneratePlaneXY1(int x1, int y1, int z, LightIntensity intensity, float textureX, float textureY, float*& fptr) {
	float textureX1 = textureX + TextSize;
	float textureY1 = textureY + TextSize;
	int x2 = x1 + 1;
	int y2 = y1 + 1;
	*fptr++ = x1;	*fptr++ = y1;	*fptr++ = z; *fptr++ = textureX; *fptr++ = textureY; *fptr++ = intensity.b001; *fptr++ = intensity.b001; *fptr++ = intensity.b001;//000,00,0
	*fptr++ = x2;	*fptr++ = y1;	*fptr++ = z; *fptr++ = textureX1; *fptr++ = textureY; *fptr++ = intensity.b101; *fptr++ = intensity.b101; *fptr++ = intensity.b101;//100,10,1
	*fptr++ = x1;	*fptr++ = y2;	*fptr++ = z; *fptr++ = textureX; *fptr++ = textureY1; *fptr++ = intensity.b011; *fptr++ = intensity.b011; *fptr++ = intensity.b011;//010,01,2
	*fptr++ = x2;	*fptr++ = y2;	*fptr++ = z; *fptr++ = textureX1; *fptr++ = textureY1; *fptr++ = intensity.b111; *fptr++ = intensity.b111; *fptr++ = intensity.b111;//110,11,3
}
void Chunk::GeneratePlaneXZ0(int x1, int y, int z1, LightIntensity intensity, float textureX, float textureY, float*& fptr) {
	float textureX1 = textureX + TextSize;
	float textureY1 = textureY + TextSize;
	int x2 = x1 + 1;
	int z2 = z1 + 1;
	*fptr++ = x1;	*fptr++ = y;	*fptr++ = z1; *fptr++ = textureX; *fptr++ = textureY; *fptr++ = intensity.b000; *fptr++ = intensity.b000; *fptr++ = intensity.b000;//000,00,0
	*fptr++ = x2;	*fptr++ = y;	*fptr++ = z1; *fptr++ = textureX1; *fptr++ = textureY; *fptr++ = intensity.b100; *fptr++ = intensity.b100; *fptr++ = intensity.b100;//100,10,1
	*fptr++ = x1;	*fptr++ = y;	*fptr++ = z2; *fptr++ = textureX; *fptr++ = textureY1; *fptr++ = intensity.b001; *fptr++ = intensity.b001; *fptr++ = intensity.b001;//001,01,2
	*fptr++ = x2;	*fptr++ = y;	*fptr++ = z2; *fptr++ = textureX1; *fptr++ = textureY1; *fptr++ = intensity.b101; *fptr++ = intensity.b101; *fptr++ = intensity.b101;//101,11,3
}
void Chunk::GeneratePlaneXZ1(int x1, int y, int z1, LightIntensity intensity, float textureX, float textureY, float*& fptr) {
	float textureX1 = textureX + TextSize;
	float textureY1 = textureY + TextSize;
	int x2 = x1 + 1;
	int z2 = z1 + 1;
	*fptr++ = x1;	*fptr++ = y;	*fptr++ = z1; *fptr++ = textureX; *fptr++ = textureY; *fptr++ = intensity.b010; *fptr++ = intensity.b010; *fptr++ = intensity.b010;//010,00,0
	*fptr++ = x1;	*fptr++ = y;	*fptr++ = z2; *fptr++ = textureX; *fptr++ = textureY1; *fptr++ = intensity.b011; *fptr++ = intensity.b011; *fptr++ = intensity.b011;//011,01,2
	*fptr++ = x2;	*fptr++ = y;	*fptr++ = z1; *fptr++ = textureX1; *fptr++ = textureY; *fptr++ = intensity.b110; *fptr++ = intensity.b110; *fptr++ = intensity.b110;//110,10,1
	*fptr++ = x2;	*fptr++ = y;	*fptr++ = z2; *fptr++ = textureX1; *fptr++ = textureY1; *fptr++ = intensity.b111; *fptr++ = intensity.b111; *fptr++ = intensity.b111;//111,11,3
}
void Chunk::GeneratePlaneYZ0(int x, int y1, int z1, LightIntensity intensity, float textureX, float textureY, float*& fptr) {
	float textureX1 = textureX + TextSize;
	float textureY1 = textureY + TextSize;
	int y2 = y1 + 1;
	int z2 = z1 + 1;
	*fptr++ = x;	*fptr++ = y1;	*fptr++ = z1; *fptr++ = textureX; *fptr++ = textureY; *fptr++ = intensity.b000; *fptr++ = intensity.b000; *fptr++ = intensity.b000;//000,00,0
	*fptr++ = x;	*fptr++ = y1;	*fptr++ = z2; *fptr++ = textureX; *fptr++ = textureY1; *fptr++ = intensity.b001; *fptr++ = intensity.b001; *fptr++ = intensity.b001;//001,01,2
	*fptr++ = x;	*fptr++ = y2;	*fptr++ = z1; *fptr++ = textureX1; *fptr++ = textureY; *fptr++ = intensity.b010; *fptr++ = intensity.b010; *fptr++ = intensity.b010;//010,10,1
	*fptr++ = x;	*fptr++ = y2;	*fptr++ = z2; *fptr++ = textureX1; *fptr++ = textureY1; *fptr++ = intensity.b011; *fptr++ = intensity.b011; *fptr++ = intensity.b011;//011,11,3
}
void Chunk::GeneratePlaneYZ1(int x, int y1, int z1, LightIntensity intensity, float textureX, float textureY, float*& fptr) {
	float textureX1 = textureX + TextSize;
	float textureY1 = textureY + TextSize;
	int y2 = y1 + 1;
	int z2 = z1 + 1;
	*fptr++ = x;	*fptr++ = y1;	*fptr++ = z1; *fptr++ = textureX; *fptr++ = textureY; *fptr++ = intensity.b100; *fptr++ = intensity.b100; *fptr++ = intensity.b100;//100,00,0
	*fptr++ = x;	*fptr++ = y2;	*fptr++ = z1; *fptr++ = textureX1; *fptr++ = textureY; *fptr++ = intensity.b110; *fptr++ = intensity.b110; *fptr++ = intensity.b110;//110,10,1
	*fptr++ = x;	*fptr++ = y1;	*fptr++ = z2; *fptr++ = textureX; *fptr++ = textureY1; *fptr++ = intensity.b101; *fptr++ = intensity.b101; *fptr++ = intensity.b101;//101,01,2
	*fptr++ = x;	*fptr++ = y2;	*fptr++ = z2; *fptr++ = textureX1; *fptr++ = textureY1; *fptr++ = intensity.b111; *fptr++ = intensity.b111; *fptr++ = intensity.b111;//111,11,3
}
void Chunk::GenerateFluidPlaneXY0(int x1, int y1, int z, LightIntensity intensity,  float*& fptr) {
	int x2 = x1 + 1;
	int y2 = y1 + 1;
	*fptr++ = x1;	*fptr++ = y1;	*fptr++ = z; *fptr++ = intensity.b000; *fptr++ = intensity.b000; *fptr++ = intensity.b000;//000,00,0
	*fptr++ = x2;	*fptr++ = y1;	*fptr++ = z;  *fptr++ = intensity.b100; *fptr++ = intensity.b100; *fptr++ = intensity.b100;//100,10,1
	*fptr++ = x1;	*fptr++ = y2;	*fptr++ = z;  *fptr++ = intensity.b010; *fptr++ = intensity.b010; *fptr++ = intensity.b010;//010,01,2
	*fptr++ = x2;	*fptr++ = y2;	*fptr++ = z;  *fptr++ = intensity.b110; *fptr++ = intensity.b110; *fptr++ = intensity.b110;//110,11,3
}
void Chunk::GenerateFluidPlaneXY1(int x1, int y1, int z, LightIntensity intensity, float*& fptr) {
	int x2 = x1 + 1;
	int y2 = y1 + 1;
	*fptr++ = x1;	*fptr++ = y1;	*fptr++ = z; *fptr++ = intensity.b001; *fptr++ = intensity.b001; *fptr++ = intensity.b001;//000,0
	*fptr++ = x2;	*fptr++ = y1;	*fptr++ = z;  *fptr++ = intensity.b101; *fptr++ = intensity.b101; *fptr++ = intensity.b101;//100,1
	*fptr++ = x1;	*fptr++ = y2;	*fptr++ = z;  *fptr++ = intensity.b011; *fptr++ = intensity.b011; *fptr++ = intensity.b011;//010,2
	*fptr++ = x2;	*fptr++ = y2;	*fptr++ = z;  *fptr++ = intensity.b111; *fptr++ = intensity.b111; *fptr++ = intensity.b111;//110,3
}
void Chunk::GenerateFluidPlaneXZ0(int x1, int y, int z1, LightIntensity intensity,  float*& fptr) {
	int x2 = x1 + 1;
	int z2 = z1 + 1;
	*fptr++ = x1;	*fptr++ = y;	*fptr++ = z1; *fptr++ = intensity.b000; *fptr++ = intensity.b000; *fptr++ = intensity.b000;//000,00,0
	*fptr++ = x2;	*fptr++ = y;	*fptr++ = z1;  *fptr++ = intensity.b100; *fptr++ = intensity.b100; *fptr++ = intensity.b100;//100,10,1
	*fptr++ = x1;	*fptr++ = y;	*fptr++ = z2;  *fptr++ = intensity.b001; *fptr++ = intensity.b001; *fptr++ = intensity.b001;//001,01,2
	*fptr++ = x2;	*fptr++ = y;	*fptr++ = z2;  *fptr++ = intensity.b101; *fptr++ = intensity.b101; *fptr++ = intensity.b101;//101,11,3
}
void Chunk::GenerateFluidPlaneXZ1(int x1, int y, int z1, LightIntensity intensity,  float*& fptr) {
	int x2 = x1 + 1;
	int z2 = z1 + 1;
	*fptr++ = x1;	*fptr++ = y;	*fptr++ = z1; *fptr++ = intensity.b010; *fptr++ = intensity.b010; *fptr++ = intensity.b010;//010,00,0
	*fptr++ = x2;	*fptr++ = y;	*fptr++ = z1;  *fptr++ = intensity.b110; *fptr++ = intensity.b110; *fptr++ = intensity.b110;//110,10,1
	*fptr++ = x1;	*fptr++ = y;	*fptr++ = z2;  *fptr++ = intensity.b011; *fptr++ = intensity.b011; *fptr++ = intensity.b011;//011,01,2
	*fptr++ = x2;	*fptr++ = y;	*fptr++ = z2;  *fptr++ = intensity.b111; *fptr++ = intensity.b111; *fptr++ = intensity.b111;//111,11,3
}
void Chunk::GenerateFluidPlaneYZ0(int x, int y1, int z1, LightIntensity intensity, float*& fptr) {
	int y2 = y1 + 1;
	int z2 = z1 + 1;
	*fptr++ = x;	*fptr++ = y1;	*fptr++ = z1; *fptr++ = intensity.b000; *fptr++ = intensity.b000; *fptr++ = intensity.b000;//000,00,0
	*fptr++ = x;	*fptr++ = y2;	*fptr++ = z1;  *fptr++ = intensity.b010; *fptr++ = intensity.b010; *fptr++ = intensity.b010;//010,10,1
	*fptr++ = x;	*fptr++ = y1;	*fptr++ = z2;  *fptr++ = intensity.b001; *fptr++ = intensity.b001; *fptr++ = intensity.b001;//001,01,2
	*fptr++ = x;	*fptr++ = y2;	*fptr++ = z2;  *fptr++ = intensity.b011; *fptr++ = intensity.b011; *fptr++ = intensity.b011;//011,11,3
}
void Chunk::GenerateFluidPlaneYZ1(int x, int y1, int z1, LightIntensity intensity,  float*& fptr) {
	int y2 = y1 + 1;
	int z2 = z1 + 1;
	*fptr++ = x;	*fptr++ = y1;	*fptr++ = z1; *fptr++ = intensity.b100; *fptr++ = intensity.b100; *fptr++ = intensity.b100;//100,00,0
	*fptr++ = x;	*fptr++ = y2;	*fptr++ = z1;  *fptr++ = intensity.b110; *fptr++ = intensity.b110; *fptr++ = intensity.b110;//110,10,1
	*fptr++ = x;	*fptr++ = y1;	*fptr++ = z2;  *fptr++ = intensity.b101; *fptr++ = intensity.b101; *fptr++ = intensity.b101;//101,01,2
	*fptr++ = x;	*fptr++ = y2;	*fptr++ = z2;  *fptr++ = intensity.b111; *fptr++ = intensity.b111; *fptr++ = intensity.b111;//111,11,3
}
void Chunk::GenerateCross(int x1, int y1, int z1,LightIntensity intensity, float textureX, float textureY, float*& fptr) {
	float textureX1 = textureX + TextSize;
	float textureY1 = textureY + TextSize;
	int x2 = x1 + 1;
	int y2 = y1 + 1;
	int z2 = z1 + 1;
	//xy to xy

	//00 to 11
	*fptr++ = x1;	*fptr++ = y1;	*fptr++ = z1; *fptr++ = textureX; *fptr++ = textureY; *fptr++ = intensity.b000; *fptr++ = intensity.b000; *fptr++ = intensity.b000;//000,00,0
	*fptr++ = x2;	*fptr++ = y2;	*fptr++ = z1; *fptr++ = textureX1; *fptr++ = textureY; *fptr++ = intensity.b110; *fptr++ = intensity.b110; *fptr++ = intensity.b110;//110,10,1
	*fptr++ = x1;	*fptr++ = y1;	*fptr++ = z2; *fptr++ = textureX; *fptr++ = textureY1; *fptr++ = intensity.b001; *fptr++ = intensity.b001; *fptr++ = intensity.b001;//001,01,2
	*fptr++ = x2;	*fptr++ = y2;	*fptr++ = z2; *fptr++ = textureX1; *fptr++ = textureY1; *fptr++ = intensity.b111; *fptr++ = intensity.b111; *fptr++ = intensity.b111;//111,11,3
	//01 to 10
	*fptr++ = x1;	*fptr++ = y2;	*fptr++ = z1; *fptr++ = textureX; *fptr++ = textureY; *fptr++ = intensity.b010; *fptr++ = intensity.b010; *fptr++ = intensity.b010;//010,00,0
	*fptr++ = x2;	*fptr++ = y1;	*fptr++ = z1; *fptr++ = textureX1; *fptr++ = textureY; *fptr++ = intensity.b100; *fptr++ = intensity.b100; *fptr++ = intensity.b100;//100,10,1
	*fptr++ = x1;	*fptr++ = y2;	*fptr++ = z2; *fptr++ = textureX; *fptr++ = textureY1; *fptr++ = intensity.b011; *fptr++ = intensity.b011; *fptr++ = intensity.b011;//011,01,2
	*fptr++ = x2;	*fptr++ = y1;	*fptr++ = z2; *fptr++ = textureX1; *fptr++ = textureY1; *fptr++ = intensity.b101; *fptr++ = intensity.b101; *fptr++ = intensity.b101;//101,11,3
}