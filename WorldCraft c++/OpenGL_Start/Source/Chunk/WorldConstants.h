#include "../Functions/TextureConstants.h"
#include <glad/glad.h>
#pragma once



#define VerticesCap 0x100000//a milion
//block data
//enumerator: each value = prev. value + 1
enum Block : unsigned char
{
	Air, Leaves, Glass, Stool, RedFlower, BlueFlower, OrangeFlower, Tallgrass, Fire, Water, Lava, Lamp, Dirt, Sand, Grass, Stone, OakWood, Ice, Bedrock, OakPlanks, Bricks, Roof, BookShelf, Furnace, SlimeBlock, Cobweb, Table, ScorchedStone, Snow, BirchWood, PalmWood, PalmPlanks, BlockCount
};
enum BlockType {
	Invisible, Solid, Cross, Fluid, Transparent
};
const float MAX_NOISE_OUTPUT = 0.707f;//sqrt(0.5)
const float NOISE_MULT = 1 / MAX_NOISE_OUTPUT;
const float NOISE_MULT_HALF = .5f / MAX_NOISE_OUTPUT;
constexpr int BIOME_SMOOTHNESS = 0x100;//0x10 chunks
const float BIOME_FREQUENCY = 1.0f / BIOME_SMOOTHNESS;

#pragma region Chunk Sizes
constexpr int ChunkScaleHor = 0x10,
ChunkScaleHorMin = ChunkScaleHor - 1,
ChunkScaleVert = 0x100,
ChunkScale2 = ChunkScaleHor * ChunkScaleHor,
ChunkScale3 = ChunkScale2 * ChunkScaleVert,
StrideY = ChunkScaleHor,
StrideZ = ChunkScale2;
#pragma endregion

constexpr int SeaLevel = 25//62
, CloudLevel = 127, Maxheight = 0xff, Bedrocklayers = 4,
MinGenHeight = 0,//SeaLevel - (Maxheight - SeaLevel) / 9.0f
MaxGenHeight = Maxheight, GenDiff = MaxGenHeight - MinGenHeight;
constexpr int BIOME_OCTAVES = 0x6;
#pragma region typedefs
typedef unsigned char BrightnessValue;
constexpr float BrightnessValueToFloat = 1.0f / 0xff;
typedef unsigned short BlockIndex;//an unsigned short that stores the position in a chunk
//typedef unsigned int BlockIndex;//an int that stores the position in a chunk
#pragma endregion
//texture sources:
//https://hammerchisel.wordpress.com/2017/11/23/woods/
//side x,y, top x, y, bottom xy
static const float textureIndices[]{
0,0,0,0,0,0,//air
TextSize * 7,LastRowY,TextSize * 7,LastRowY,TextSize * 7,LastRowY,//leaves
TextSize * 14,LastRowY,TextSize * 14,LastRowY,TextSize * 14,LastRowY,//glass
TextSize * 2,TextSize * 14,TextSize * 3,TextSize * 14,TextSize * 4,TextSize * 14,//stool
TextSize * 10,LastRowY,0,0,0,0,//red flower
TextSize * 5,TextSize * 14,0,0,0,0,//blue flower
TextSize * 6,TextSize * 14,0,0,0,0,//orange flower
TextSize * 11,LastRowY,0,0,0,0,//tall grass
TextSize * 11,TextSize * 14,0,0,0,0,//fire
TextSize * 12,LastRowY,TextSize * 12,LastRowY,TextSize * 12,LastRowY,//water
TextSize * 7,TextSize * 14,TextSize * 7,TextSize * 14,TextSize * 7,TextSize * 14,//lava
TextSize * 9,TextSize * 14,TextSize * 9,TextSize * 14,TextSize * 9,TextSize * 14,//lamp
TextSize * 2,LastRowY,TextSize * 2,LastRowY,TextSize * 2,LastRowY,//dirt
TextSize * 4,LastRowY,TextSize * 4,LastRowY,TextSize * 4,LastRowY,//sand
TextSize * 1,LastRowY,0,LastRowY,TextSize * 2,LastRowY,//grass
TextSize * 3,LastRowY,TextSize * 3,LastRowY,TextSize * 3,LastRowY,//stone
TextSize * 5,LastRowY,TextSize * 6,LastRowY,TextSize * 6,LastRowY,//wood
TextSize * 13,LastRowY,TextSize * 13,LastRowY,TextSize * 13,LastRowY,//Ice
TextSize * 8,TextSize * 14,TextSize * 8,TextSize * 14,TextSize * 8,TextSize * 14,//bedrock
TextSize * 15,LastRowY,TextSize * 15,LastRowY,TextSize * 15,LastRowY,//oak planks
TextSize * 12,TextSize * 14,TextSize * 12,TextSize * 14,TextSize * 12,TextSize * 14,//bricks
TextSize * 10,TextSize * 14,TextSize * 10,TextSize * 14,TextSize * 10,TextSize * 14,//roof
TextSize * 14,TextSize * 14,TextSize * 13,TextSize * 14,TextSize * 13,TextSize * 14,//book shelf
0,TextSize * 14,TextSize,TextSize * 14,TextSize,TextSize * 14,//furnace
TextSize * 15,TextSize * 14,TextSize * 15,TextSize * 14,TextSize * 15,TextSize * 14,//slime block
0,TextSize * 13,0,TextSize * 13,0,TextSize * 13,//cobweb
TextSize,TextSize * 13,TextSize * 3,TextSize * 14,TextSize * 4,TextSize * 14,//table
TextSize * 2,TextSize * 13,TextSize * 2,TextSize * 13,TextSize * 2,TextSize * 13,//scorchedstone
TextSize * 3,TextSize * 13,TextSize * 3,TextSize * 13,TextSize * 3,TextSize * 13,//snow
TextSize * 5,TextSize * 13,TextSize * 4,TextSize * 13,TextSize * 4,TextSize * 13,//birch wood
TextSize * 7,TextSize * 13,TextSize * 6,TextSize * 13,TextSize * 6,TextSize * 13,//palm wood
TextSize * 8,TextSize * 13,TextSize * 8,TextSize * 13,TextSize * 8,TextSize * 13,//palm planks
};
static const BlockType GetBlockType[]{
Invisible,		//air
Transparent,	//leaves
Transparent,	//glass
Transparent,	//stool
Cross,		//red flower
Cross,		//blue flower
Cross,		//orange flower
Cross,		//tall grass
Cross,		//fire
Fluid,		//water
Fluid,		//lava
Solid,		//lamp
Solid,		//dirt
Solid,		//sand
Solid,		//grass
Solid,		//stone
Solid,		//wood
Solid,		//Ice
Solid,		//bedrock
Solid,		//oak planks
Solid,		//bricks
Solid,		//roof
Solid,		//bookshelf
Solid,		//furnace
Solid,		//slime block
Cross,		//cobweb
Transparent,//table
Solid,		//scorched stone
Solid,		//snow
Solid,		//birch wood
Solid,		//palm wood
Solid,		//palm planks
};
static const float Opacity[]{
1,		//air
0.75f,	//leaves
0.9f,	//glass
0.2f,	//stool
0.9f,	//red flower
0.9f,	//blue flower
0.9f,	//orange flower
0.85f,	//tall grass
1,		//fire
0.95f,	//water
1,		//lava
1,		//lamp
0,		//dirt
0,		//sand
0,		//grass
0,		//stone
0,		//wood
0,		//Ice
0,		//bedrock
0,		//oak planks
0,		//bricks
0,		//roof
0,		//bookshelf
0,		//furnace
0,		//slime block
0.9f,	//cobweb
0.3f,	//table
0,		//scorched stone
0,		//snow
0,		//birch wood
0,		//palm wood
0,		//palm planks
};
static const float GetBounceMultiplier[]
{
0,		//air
0,		//leaves
0,		//glass
-.3f,	//stool
0,		//red flower
0,		//blue flower
0,		//orange flower
0,		//tall grass
0,		//fire
0,		//water
0,		//lava
0,		//lamp
0,		//dirt
0,		//sand
0,		//grass
0,		//stone
0,		//wood
0,		//Ice
0,		//bedrock
0,		//oak planks
0,		//bricks
0,		//roof
0,		//bookshelf
0,		//furnace
-.8f,		//slime block
0,		//cobweb
0,		//table
0,		//scorched stone
0,		//snow
0,		//birch wood
0,		//palm wood
0,		//palm planks
};
static const bool IsRaycastVisible[]
{
false,	//air
true,	//leaves
true,	//glass
true,	//stool
true,	//red flower
true,	//blue flower
true,	//orange flower
true,	//tall grass
true,	//fire
false,	//water
false,	//lava
true,	//lamp
true,	//dirt
true,	//sand
true,	//grass
true,	//stone
true,	//wood
true,	//Ice
true,	//bedrock
true,	//oak planks
true,	//bricks
true,	//roof
true,	//bookshelf
true,	//furnace
true,	//slime block
true,	//cobweb
true,	//table
true,	//scorched stone
true,	//snow
true,	//birch wood
true,	//palm wood
true,	//palm planks
};
static const bool IsSolidBlock[]{
false,	//air
true,	//leaves
true,	//glass
true,	//stool
false,	//red flower
false,	//blue flower
false,	//orange flower
false,	//tall grass
false,	//fire
false,	//water
false,	//lava
true,	//lamp
true,	//dirt
true,	//sand
true,	//grass
true,	//stone
true,	//wood
true,	//Ice
true,	//bedrock
true,	//oak planks
true,	//bricks
true,	//roof
true,	//bookshelf
true,	//furnace
true,	//slime block
false,	//cobweb
true,	//table
true,	//scorched stone
true,	//snow
true,	//birch wood
true,	//palm wood
true,	//palm planks
};
static const bool DrawAllSides[]{
false,	//air
false,	//leaves
false,	//glass
false,	//stool
false,	//red flower
false,	//blue flower
false,	//orange flower
false,	//tall grass
false,	//fire
true,	//water
true,	//lava
true,	//lamp
true,	//dirt
true,	//sand
true,	//grass
true,	//stone
true,	//wood
true,	//Ice
true,	//bedrock
true,	//oak planks
true,	//bricks
true,	//roof
true,	//bookshelf
true,	//furnace
true,	//slime block
false,	//cobweb
false,	//table
true,	//scorched stone
true,	//snow
true,	//birch wood
true,	//palm wood
true,	//palm planks
};

const int Octaves = 6;//the amount of noise layers
const int Smoothness = 0x100;//smallest layer: 4
const float FREQUENCY = 1.0f / Smoothness;

//indicates wether the block has air in it
static const bool HasAir[]{
true,	//air
false,	//leaves
false,	//glass
false,	//stool
true,	//red flower
true,	//blue flower
true,	//orange flower
true,	//tall grass
true,	//fire
false,	//water
false,	//lava
false,	//lamp
false,	//dirt
false,	//sand
false,	//grass
false,	//stone
false,	//wood
false,	//Ice
false,	//bedrock
false,	//oak planks
false,	//bricks
false,	//roof
false,	//bookshelf
false,	//furnace
false,	//slime block
true,	//cobweb
false,	//table
false,	//scorched stone
false,	//snow
false,	//birch wood
false,	//palm wood
false,	//palm planks
};
//indicates wether the block is visible when surrounded
static const bool IsVisibleSurrounded[]{
true,	//air
true,	//leaves
true,	//glass
true,	//stool
true,	//red flower
true,	//blue flower
true,	//orange flower
true,	//tall grass
true,	//fire
true,	//water
true,	//lava
false,	//lamp
false,	//dirt
false,	//sand
false,	//grass
false,	//stone
false,	//wood
false,	//Ice
false,	//bedrock
false,	//oak planks
false,	//bricks
false,	//roof
false,	//bookshelf
false,	//furnace
false,	//slime block
true,	//cobweb
true,	//table
false,	//scorched stone
false,	//snow
false,	//birch wood
false,	//palm wood
false,	//palm planks
};
constexpr int TextIndiceCount = 6;//6 indices per texture
enum Biome
{
	OCEAN,
	BEACH,
	SCORCHED,
	BARE,
	TUNDRA,
	SNOW,
	TEMPERATE_DESERT,
	SHRUBLAND,
	TAIGA,
	GRASSLAND,
	TEMPERATE_DECIDUOUS_FOREST,
	TEMPERATE_RAIN_FOREST,
	SUBTROPICAL_DESERT,
	TROPICAL_SEASONAL_FOREST,
	TROPICAL_RAIN_FOREST,
	BIOME_COUNT
};
enum Artifact : unsigned char {
	aOakTree,
	aBirchTree,
	aPalmTree,
	aPineTree,
	aRedFlower,
	aBlueFlower,
	aOrangeFlower,
	ArtifactCount
};
//x: artifact
//y: biome
//oak tree, birch tree, palm tree, pine tree, red flower, blue flower, orange flower
static const int chance[ArtifactCount * BIOME_COUNT]
{
	0,	0,	0,	0,	0,	0,	0,		//ocean
	0,	0,	30,	30,	0,	0,	0,		//beach
	0,	0,	0,	0,	0,	0,	10,		//scorched
	0,	0,	0,	2,	2,	10,	10,		//bare
	2,	2,	2,	20,	20,	30,	40,		//tundra
	0,	0,	0,	10,	15,	30,	40,		//snow
	0,	0,	10,	10,	10,	10,	10,		//temperate desert
	10,	10,	10,	10,	30,	30,	30,		//shrubland
	10,	10,	10,	30,	50,	70,	90,		//taiga
	5,	10,	20,	20,	30,	30,	30,		//grassland
	30,	30,	50,	50,	90,	110,120,	//temperate deciduous forest
	20,	20,	40,	40,	70,	90,	100,	//temperate rain forest
	20,	20,	40,	40,	70,	90,	100,	//subtropical desert
	10,	30,	40,	40,	70,	90,	100,	//subtropical seasonal forest
	40,	40,	60,	60,	70,	80,	80,		//tropical rain forest
};
static const GLubyte BiomeColor[BIOME_COUNT * 4]
{
	67,67,122,0xff,		//ocean
	160,145,119,0xff,	//beach
	85,85,85,0xff,		//scorched
	136,136,136,0xff,	//bare
	188,188,170,0xff,	//tundra
	222,222,229,0xff,	//snow
	201,210,155,0xff,	//temperate desert
	136,153,119,0xff,	//shrubland
	153,171,119,0xff,	//taiga
	136,171,85,0xff,	//grassland
	103,147,89,0xff,	//temperate deciduous forest
	67,136,85,0xff,		//temperate rain forest
	210,185,139,0xff,	//subtropical desert
	86,153,68,0xff,		//subtropical seasonal forest
	51,119,85,0xff,		//tropical rain forest
};
static const Block TopBlock[BIOME_COUNT]
{
	Sand,			//ocean
	Sand,			//beach
	ScorchedStone,	//scorched
	Stone,			//bare
	Snow,			//tundra
	Snow,			//snow
	Sand,			//temperate desert
	Grass,			//shrubland
	Grass,			//taiga
	Grass,			//grassland
	Grass,			//temperate deciduous forest
	Grass,			//temperate rain forest
	Sand,			//subtropical desert
	Grass,			//subtropical seasonal forest
	Grass,			//tropical rain forest
};
static const Block LayerBlock[BIOME_COUNT]
{
	Sand,			//ocean
	Sand,			//beach
	ScorchedStone,	//scorched
	Stone,			//bare
	Snow,			//tundra
	Stone,			//snow
	Sand,			//temperate desert
	Stone,			//shrubland
	Stone,			//taiga
	Dirt,			//grassland
	Dirt,			//temperate deciduous forest
	Dirt,			//temperate rain forest
	Sand,			//subtropical desert
	Dirt,			//subtropical seasonal forest
	Dirt,			//tropical rain forest
};