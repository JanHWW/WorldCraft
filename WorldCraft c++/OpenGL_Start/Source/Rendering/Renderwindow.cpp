#include "RenderWindow.h"
//#include <FreeImage/FreeImage.h>
#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif
constexpr float
AirFriction = .98f,
AirFrictionMultiplier = (1 - AirFriction),
GroundFriction = .95f,
GroundFrictionMultiplier = (1 - GroundFriction),
FluidFriction = .9f,
FluidFrictionMultiplier = (1 - FluidFriction);
float ScreenProportion;
constexpr int populationMargin = 1;//population margin: 1 chunk
constexpr int SaveDiametre = 0x42;//0x10;//the range in which chunks remain in work memory
constexpr int RenderDistance = 0x20,//0x20,//0x6,
#if SaveDiametre < ((RenderDistance + populationMargin) * 2)
#error "SaveDiametre has to be more than (RenderDistance + populationMargin) * 2"
#endif
RenderDistance2 = RenderDistance * RenderDistance;//render distance in chunks
constexpr int LoadBlocks = SaveDiametre * ChunkScaleHor;//render distance in blocks
constexpr int LoadChunksMin = SaveDiametre - 1, SaveDiametre2 = SaveDiametre * SaveDiametre, HalfLoadBlocks = LoadBlocks / -2;//a
constexpr float MaxDistance = (RenderDistance - 1) * ChunkScaleHor;//INFINITY;
constexpr float MaxDistance2 = MaxDistance * MaxDistance;
Block SelectedBlock;
int SelectedBlockX, SelectedBlockY, SelectedBlockZ;
int AdjacentBlockX, AdjacentBlockY, AdjacentBlockZ;

int DesktopWidth, DesktopHeight;
int WindowWidth, WindowHeight;
int WindowX = 0, WindowY = 0;

bool DrawControls = true;
glm::vec3 LookDirection;
#pragma region Public_Variables
GLFWwindow* OutputWindow;
GLFWmonitor* OutputMonitor;
//glm::mat4 TransForm; // make sure to initialize matrix to identity matrix first
glm::vec3 Position;
//transforms
glm::mat4 projection, view;
//looking over positive x -axis
ViewFrustum ActiveFrustum;
float
FOV = 90, Pitch, Yaw,
EyeHeight = 1.62f,
SecondsElapsed, LastTime, LastUpdateTime,
BuildRadius = 0x10,
BackGroundR = .66f, BackGroundG = .78f, BackGroundB = 1.0f,
WaterR = .16f, WaterG = .22f, WaterB = .71f;//render settings
GLuint MobVAO, MobVBO;
GLuint IBO;	//index buffer object
//GLuint OutWaterArrays[RenderDistance2 * RenderDistance2];
//GLuint WaterArrays[RenderDistance2 * RenderDistance2];
//float smoothness = 0x40;
int LayerCount = 4;
int PlusX, PlusY, PlusChunkY,PlusChunkX;
bool CanJump = false;
bool IsVisible[SaveDiametre2];//determines wether a chunk is visible
Chunk* ChunkTiles[SaveDiametre2];//chunks formatted as a 2d array
std::list<Chunk*> Chunk1d[SaveDiametre2 * 2];//list with chunks unformatted
constexpr int MAX_MOBS = 0x10000;
std::list<Mob*> mobs[MAX_MOBS];
bool ClickedLastFrame;
#pragma region Graphics
Shader* WorldShader;
Shader* WaterShader;
Control* Inventory;
Texture* txtrCrosshair;
Texture* txtrInventorySlot;
Texture* BlockTextures;
ArrayMerger* DynamicMeshMerger3D;//mesh merger for mobs
#pragma endregion
Block ActiveBlock;
#pragma region Psychics
glm::vec3 Speed;
bool Gravity = false;
float height;
#pragma endregion
#pragma endregion

float lastx,lasty;
#pragma region publics
char* ScreenShotPath;
//static char* ScreenShotPath = (char*)"";//not const because of internal linkage
float MouseSensitivity = 0.1f;
#pragma endregion

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	int count = -(int)yoffset;
	int NewBlock = ActiveBlock + count;
	if (ActiveBlock != NewBlock) {
		if (NewBlock < 0)NewBlock = 0;
		else if (NewBlock >= BlockCount)NewBlock = BlockCount - 1;
		ActiveBlock = (Block)NewBlock;
		if (NewBlock == 0) {
			Inventory->texture = txtrInventorySlot->Location;
			Inventory->SetTextureCoords();
		}
		else {
			Inventory->texture = BlockTextures->Location;
			float 
				tx = textureIndices[NewBlock*TextIndiceCount],
				ty = textureIndices[NewBlock*TextIndiceCount + 1];
			Inventory->SetTextureCoords(tx, ty, tx + TextSize, ty + TextSize);
		}
	}

}
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	float
		MouseDX = xpos - lastx,
		MouseDY = ypos - lasty;
	Yaw += MouseDX * MouseSensitivity;
	Pitch -= MouseDY * MouseSensitivity;
	lastx = xpos;
	lasty = ypos;
}
void click_callback(GLFWwindow* window, int button, int PressOrRelease, int Modifiers) {
	if (!ClickedLastFrame) {
		if (SelectedBlock > Air && PressOrRelease == MouseDown) {
			if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) //spawn pig
			{
				mobs->push_back(new Pig(Position));
			}
			else if (button == DigButton) {
				Dig();
			}
			else if (button == PlaceButton) {
				Place();
			}
		}
	}
}
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the DBG_NEW window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, WindowWidth = width, WindowHeight = height);
}
void Place() {
	ClickedLastFrame = true;
	SetBlock(AdjacentBlockX, AdjacentBlockY, AdjacentBlockZ, ActiveBlock, true);
}
void Dig() {
	ClickedLastFrame = true;
	SetBlock(SelectedBlockX, SelectedBlockY, SelectedBlockZ, Air, true);
}
// Get the horizontal and vertical screen sizes in pixel
void GetDesktopResolution(int& horizontal, int& vertical)
{
	RECT desktop;
	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	horizontal = desktop.right;
	vertical = desktop.bottom;
}
//determines per chunk if it will be drawn, crops the chunks to view
void CropView() {
	ActiveFrustum.update(projection * view);
	int index = 0;
	float px = (Position.x / ChunkScaleHor) - PlusChunkX;
	float py = (Position.y / ChunkScaleHor) - PlusChunkY;
	glm::vec3 ChunkSize = glm::vec3(ChunkScaleHor, ChunkScaleHor, ChunkScaleVert);
	for (int j = 0, ChunkY = PlusY; j < SaveDiametre; j++, ChunkY += ChunkScaleHor)
	{
		float dy0 = j - py, dy1 = dy0 + 1;
		dy0 *= dy0;
		dy1 *= dy1;
		float dy = dy0 < dy1 ? dy0 : dy1;//squared distance on the y-axis to the camera
		for (int i = 0, ChunkX = PlusX; i < SaveDiametre; i++, index++, ChunkX += ChunkScaleHor)
		{
			float dx0 = i - px, dx1 = dx0 + 1;
			dx0 *= dx0;
			dx1 *= dx1;
			float dx = dx0 < dx1 ? dx0 : dx1;//squared distance on the x-axis to the camera

			float DistanceToPlayer2 = dx + dy;
			if (DistanceToPlayer2 > RenderDistance2) {
				IsVisible[index] = false;
				continue;
			}
			//frustum culling
			IsVisible[index] = ActiveFrustum.isBoxInFrustum(glm::vec3(PlusX + i * ChunkScaleHor, PlusY + j * ChunkScaleHor, 0), ChunkSize);
		}
	}
}
void SetDirection() {
	if (Pitch > 89.0f)
		Pitch = 89.0f;
	if (Pitch < -89.0f)
		Pitch = -89.0f;
	float RadiansPitch = glm::radians(Pitch);
	float RadiansYaw = glm::radians(-Yaw);
	float CosYaw = cos(RadiansYaw);
	float SinYaw = sin(RadiansYaw);
	float CosPitch = cos(RadiansPitch);
	float SinPitch = sin(RadiansPitch);
	LookDirection = glm::vec3(CosPitch * CosYaw, CosPitch * SinYaw, SinPitch);
	float l = LookDirection.length();
}
void Raycast() {
	glm::vec3 EyePos = Position;
	EyePos.z += EyeHeight;
	walk_grid(EyePos, EyePos + LookDirection * BuildRadius, SelectedBlockX, SelectedBlockY, SelectedBlockZ, AdjacentBlockX, AdjacentBlockY, AdjacentBlockZ, SelectedBlock);
}
bool walk_grid(glm::vec3 p0, glm::vec3 p1, int &selx, int &sely, int &selz, int &adjx, int &adjy, int &adjz, Block &block)
{
	//https://www.redblobgames.com/grids/line-drawing.html
	//http://www.cse.yorku.ca/~amana/research/grid.pdf
	float dx = p1.x - p0.x, dy = p1.y - p0.y, dz = p1.z - p0.z;
	int sign_x = dx > 0 ? 1 : -1, sign_y = dy > 0 ? 1 : -1, sign_z = dz > 0 ? 1 : -1;
	//not dx * sign_x, because you can get -0 and -inf
	float nx = abs(dx), ny = abs(dy), nz = abs(dz);
	int px = floor(p0.x);
	int py = floor(p0.y);
	int pz = floor(p0.z);
	int dimension = 0;
	float deltaX = 1 / nx;
	float deltaY = 1 / ny;
	float deltaZ = 1 / nz;
	float vx = dx > 0 ? px + 1 - p0.x : p0.x - px;
	float vy = dy > 0 ? py + 1 - p0.y : p0.y - py;
	float vz = dz > 0 ? pz + 1 - p0.z : p0.z - pz;
	vx *= deltaX;
	vy *= deltaY;
	vz *= deltaZ;
	int count = 0;
	while(count < BuildRadius) {
		//points.push(new Point(p.x, p.y));
		//check point
		Block b = GetBlock(px, py, pz);
		if (IsRaycastVisible[b]) {
			selx = px;
			sely = py;
			selz = pz;
			adjx = px;
			adjy = py;
			adjz = pz;
			block = b;
			if (dimension == 1) {//last dimension: x
				adjx = SelectedBlockX - sign_x;
			}
			else if (dimension == 2) {//last dimension: y
				adjy = SelectedBlockY - sign_y;
			}
			else if (dimension == 3) {//last dimension: z
				adjz = SelectedBlockZ - sign_z;
			}
			return true;
		}
		if (vx < vy && vx < vz) {
			// next step is x
			px += sign_x;
			dimension = 1;
			vx += deltaX;
		}
		else if (vy < vz) {
			// next step is y
			py += sign_y;
			dimension = 2;
			vy += deltaY;
		}
		else {
			// next step is z
			pz += sign_z;
			dimension = 3;
			vz += deltaZ;
		}
		count++;
	}
	block = Air;
	return false;
}
void ReloadTerrain() {
	float px = Position.x, py = Position.y, pz = Position.z;
	int intposx = floor(Position.x / ChunkScaleHor);
	int intposy = floor(Position.y / ChunkScaleHor);
	int NewPlusX = intposx * ChunkScaleHor + HalfLoadBlocks;
	int NewPlusY = intposy * ChunkScaleHor + HalfLoadBlocks;
	bool changed = PlusX != NewPlusX || PlusY != NewPlusY;
	PlusX = NewPlusX;
	PlusY = NewPlusY;
	PlusChunkX = PlusX / ChunkScaleHor;
	PlusChunkY = PlusY / ChunkScaleHor;
	if (changed)
	{
		RemoveOutBounds();
		Documentate();
	}
}
void RemoveOutBounds() {
	//int count = Chunk1d->size();
	auto i = Chunk1d->begin();
	while (i != Chunk1d->end())
	{
		Chunk* chunk = *i;
		int
			cx = chunk->xPos - PlusChunkX,
			cy = chunk->yPos - PlusChunkY;
		if (cx < 0 || cx >= SaveDiametre || cy < 0 || cy >= SaveDiametre) //out of bounds
		{
			//chunk->Delete();
			delete chunk;
			i = Chunk1d->erase(i);
		}
		else {
			i++;
		}
	}
}
void PopulateNearestNullChunk()
{
	int ix = 0, iy = 0;
	//float max = LoadChunks / 2 - .5;
	//max *= max;
	float NearestDistance = INFINITY;
	//int bytesize = LoadChunks2 * sizeof(Chunk);
	float px = (Position.x / ChunkScaleHor) - PlusChunkX;
	float py = (Position.y / ChunkScaleHor) - PlusChunkY;
	int index = 0;
	for (int j = 0; j < SaveDiametre; j++)
	{
		float yoff = j - py;
		yoff *= yoff;
		for (int i = 0; i < SaveDiametre; i++,index++)
		{
			if ((ChunkTiles[index] == NULL || !ChunkTiles[index]->Populated) && IsVisible[index])
			{
				float xoff = i - px;
				float d = xoff * xoff + yoff;
				if (d < NearestDistance)
				{
					NearestDistance = d;
					ix = i;
					iy = j;
				}
			}
		}
	}
	if (NearestDistance < INFINITY)
	{
		//check if surrounding chunks can populate
		int MinX = ix - populationMargin;
		int MinY = iy - populationMargin;
		int MaxX = ix + populationMargin;
		int MaxY = iy + populationMargin;
		for (int ActiveY = MinY; ActiveY <= MaxY; ActiveY++)
		{
			for (int ActiveX = MinX; ActiveX <= MaxX; ActiveX++)
			{
				int index = ActiveY * SaveDiametre + ActiveX;
				Chunk* ActiveChunk = ChunkTiles[index];
				if (ActiveChunk == NULL) {//generate chunk blocks
					ActiveChunk = DBG_NEW Chunk(ActiveX + PlusChunkX, ActiveY + PlusChunkY);
					Chunk1d->push_back(ActiveChunk);
					ChunkTiles[index] = ActiveChunk;
				}
			}
		}
		Chunk* ChunkToPopulate = ChunkTiles[ix + iy * SaveDiametre];
		ChunkToPopulate->Populate();
	}
}
//fill tile array with loaded chunks
void Documentate() {
	for (int i = 0; i < SaveDiametre2; i++) {
		ChunkTiles[i] = NULL;
	}

	//Chunk** ptr = ChunkTiles;//reset chunk array
	//Chunk** End = ptr + LoadChunks2;
	//while (ptr<End)
	//{
	//	*ptr++ = NULL;
	//}
	for (auto i = Chunk1d->begin(); i != Chunk1d->end(); i++) {
		Chunk* chunk = *i;
		ChunkTiles[chunk->xPos - PlusChunkX + (chunk->yPos - PlusChunkY) * SaveDiametre] = chunk;
	}

}
//creates the render window, returns wether success
bool CreateRenderWindow(int Width, int Height, const char* Title)
{
	ScreenShotPath = new char[1]{ 0 };
	OutputMonitor = glfwGetPrimaryMonitor();
	//const GLFWvidmode* mode = glfwGetVideoMode(OutputMonitor);

	//glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	//glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	//glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	//glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

	/* Create a windowed mode window and its OpenGL context */
	OutputWindow = glfwCreateWindow(WindowWidth = Width, WindowHeight = Height, Title, NULL, NULL);
	//OutputWindow = glfwCreateWindow(mode->width, mode->height, Title, ActiveMonitor, NULL);
	lastx = WindowWidth / 2.0;
	lasty = WindowHeight / 2.0;
	if (OutputWindow == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	// glfw: initialize and configure
		// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

		/* Make the window's context current */
	glfwMakeContextCurrent(OutputWindow);
	glfwSetFramebufferSizeCallback(OutputWindow, framebuffer_size_callback);
	glfwSetCursorPosCallback(OutputWindow, mouse_callback);
	glfwSetMouseButtonCallback(OutputWindow, click_callback);
	glfwSetScrollCallback(OutputWindow, scroll_callback);
	//glfwSetCursorPosCallback(window, mouse_callback);
	//glfwSetScrollCallback(window, scroll_callback);

	//uncomment this to tell GLFW to capture our mouse
	glfwSetInputMode(OutputWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	// configure global opengl state
		// -----------------------------
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND); //allows transparency
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_ONE, GL_ZERO);
	//https://software.intel.com/en-us/forums/graphics-driver-bug-reporting/topic/591322
	Position = glm::vec3(0, 0, Maxheight);
	GLFWwindow* window = OutputWindow;
	//unsigned int buffer;
	ScreenProportion = (float)WindowWidth / (float)WindowHeight;
#pragma region Generate Mesh
	//generate buffer object
	const GLuint MAX_VERTICES = 0x10000 * 6;//786 thousand
	GLuint* indices = new GLuint[MAX_VERTICES];
	GLuint* endptr = indices + MAX_VERTICES;
	GLuint* ptr = indices;
	GLuint off = 0;
	while (ptr < endptr)
	{
		*ptr++ = off;		//0
		*ptr++ = off + 1;	//1
		*ptr++ = off + 2;	//2
		*ptr++ = off + 3;	//3
		*ptr++ = off + 2;	//2
		*ptr++ = off + 1;	//1
		off += 4;
	}
	glGenBuffers(1, &IBO);
	//glEnableClientState(GL_INDEX_ARRAY);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	//copy to GPU
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * MAX_VERTICES, indices, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//destroy array
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	delete[] indices;
#pragma region Generate3D
#pragma region OutWater
	WorldShader = DBG_NEW Shader("Data/shaders/camera.vs", "Data/shaders/camera.fs");
	GenVerticeBuffers(&MobVBO, &MobVAO);
	SolidParameters();
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	BlockTextures = DBG_NEW Texture("Data\\HD2.png");

	DynamicMeshMerger3D = DBG_NEW ArrayMerger();
	WorldShader->unbind();
#pragma endregion
#pragma region Water
	WaterShader = DBG_NEW Shader("Data/shaders/fluid.vs", "Data/shaders/fluid.fs");
#pragma endregion
#pragma endregion
	InitializeGUI();
	GenerateControls();
#pragma endregion
	//MouseSensitivity = 0.1f;
	ActiveFrustum = ViewFrustum();
	return true;
}
void GenerateControls() {
	txtrCrosshair = new Texture("Data/Crosshair.png");
	const float crosshairsize2 = .01, InventorySlotSize = .05;
	float crosshairw2 = crosshairsize2, crosshairh2 = crosshairsize2 * ScreenProportion;
	Control* crosshair = new Control(0.5f - crosshairw2, 0.5f - crosshairh2, crosshairw2 * 2, crosshairh2 * 2, txtrCrosshair->Location);
	txtrInventorySlot = new Texture("data/inventoryslot.png");
	float InventorySlotWidth = InventorySlotSize, InventorySlotHeight = InventorySlotSize * ScreenProportion;
	Inventory = new Control(.5f - InventorySlotWidth * .5f, .1f, InventorySlotWidth, InventorySlotHeight, txtrInventorySlot->Location);
}
void DeleteChunks()
{
	Chunk1d->clear();
	for (int i = 0; i < SaveDiametre2; i++) {
		delete ChunkTiles[i];
		ChunkTiles[i] = NULL;
	}
	cout << "deleted all loaded chunks and actions" << endl;
}
#define cast reinterpret_cast<const char*>
void Save(const char* path) {
	ofstream stream;//output file stream
	stream.open(path, ios::binary);
	if (stream.good()) {
		stream.seekp(0, stream.beg);
		const int fs = sizeof(float);
		//world options
		stream.write(cast(&Seed), sizeof(int));//only world option to write: seed

		//events
		int count = GetActionContainerCount();
		stream.write(cast(&count), sizeof(int));
		for (int i = 0; i < count; i++) {
			ActionContainer* ActiveContainer = getActionContainerPointer(i);
			stream.write(cast(&ActiveContainer->posX), sizeof(int));
			stream.write(cast(&ActiveContainer->posY), sizeof(int));
			list<Action*>* actions = ActiveContainer->actions;
			int size = (int)actions->size();
			stream.write(cast(&size), sizeof(int));
			for (auto j = actions->begin(); j != actions->end(); j++) {
				Action* action = *j;
				stream.write(cast(&action->block), sizeof(Block));
				stream.write(cast(&action->index), sizeof(BlockIndex));
			}
		}

	}
	else {
		cout << "Faillure. Filename was bad" << endl;
	}
	stream.close();
}
void Open(const char* path) {
	ifstream stream;//input file stream
	stream.open(path, ios::binary);
	if (stream.good()) {
		stream.seekg(0, stream.beg);
		const int fs = sizeof(float);
		//world options
		stream.read((char*)&Seed, sizeof(int));
		//initialize terrain generator by seed
		InitializeTerrain();
		//events
		DeleteChunks();
		int count;
		stream.read((char*)& count, sizeof(int));
		for (int i = 0; i < count; i++) {
			int posX, posY;
			stream.read((char*)& posX, sizeof(int));
			stream.read((char*)& posY, sizeof(int));
			ActionContainer* ActiveContainer = new ActionContainer(posX, posY);
			list<Action*>* actions = ActiveContainer->actions;
			int size;
			stream.read((char*)& size, sizeof(int));
			for (int j = 0; j < size; j++) {
				BlockIndex index;
				Block block;
				stream.read((char*)& block, sizeof(Block));
				stream.read((char*)& index, sizeof(BlockIndex));
				actions->push_back(new Action(index, block));
			}
		}
	}
	else {
		cout << "Faillure. Filename was bad" << endl;
	}
	stream.close();
}
bool close;
bool IsFullScreen() 
{
	return glfwGetWindowMonitor(OutputWindow) != nullptr;
}
void SetFullScreen(bool fullscreen)
{
	if (IsFullScreen() == fullscreen)
		return;

	if (fullscreen)
	{
		//store location and size
		glfwGetWindowPos(OutputWindow, &WindowX, &WindowY);
		glfwGetWindowSize(OutputWindow, &WindowWidth, &WindowHeight);

		// get resolution of monitor
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

		// switch to full screen
		glfwSetWindowMonitor(OutputWindow, OutputMonitor, 0, 0, mode->width, mode->height, 0);
	}
	else
	{
		// restore last window size and position
		glfwSetWindowMonitor(OutputWindow, nullptr, WindowX, WindowY, WindowWidth, WindowHeight, 0);
	}

	//Viewport needs to be updated
}
bool Run() {
	close = false;
	LastTime = glfwGetTime();

#ifndef  _DEBUG
	SetFullScreen(true);
#endif // ! DEBUG

	const int w = 200, h = 200;
	//create noise map
	GLubyte* NoiseMap = new GLubyte[w * h * 4];
	GLubyte* ptr = NoiseMap;
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			Biome ActiveBiome = GetBiome(x , y );
			*ptr++ = BiomeColor[ActiveBiome * 4];
			*ptr++ = BiomeColor[ActiveBiome * 4 + 1];
			*ptr++ = BiomeColor[ActiveBiome * 4 + 2];
			*ptr++ = 0xff;
		}
	}
	Texture* texture = new Texture(w, h, 4, NoiseMap);
	Control* control = new Control(0, 0,.2f/ ScreenProportion, .2f, texture->Location, 0, 1, 0, 1);
	while (!close)
	{
		if (glfwWindowShouldClose(OutputWindow)) 
		{
			cout << "a";
			return true; 
		}
		ReloadTerrain();
		//generate nearest chunk
		PopulateNearestNullChunk();
		//sleep
		float NewTime = glfwGetTime();
		
		for (int i = 0; LastUpdateTime < NewTime; i++) 
		{
			//if (i < 10) {
				// input
				processInput();//max 10 frames at once
			//}
				LastUpdateTime += TickTime;
		}
		SecondsElapsed = NewTime - LastTime;
		int milisecSleep = (int)(MSFrame - SecondsElapsed * 1000);
		std::this_thread::sleep_for(std::chrono::milliseconds(milisecSleep));
		LastTime = glfwGetTime();
		SetDirection();
		Raycast();
		int focused = glfwGetWindowAttrib(OutputWindow, GLFW_FOCUSED);
		//if (focused) 
		//{
			Draw();
		//}
		ClickedLastFrame = false;
		// glfw: poll IO events (keys pressed/released, mouse moved etc.)
		//glfwPollEvents();
		// -------------------------------------------------------------------------------
		/* Swap front and back buffers */
		glfwSwapBuffers(OutputWindow);
		// Poll for and process events
		glfwPollEvents();
	}
	SetFullScreen(false);
	return false;
}
void Delete() {
	// optional: de-allocate all resources once they've outlived their purpose:
// ------------------------------------------------------------------------
	for (int i = 0; i < SaveDiametre2; i++) {
		delete ChunkTiles[i];
	}
	Chunk1d->clear();
	delete WorldShader;
	delete BlockTextures;
	DeleteGUI();

	//glDeleteBuffers(1, &EBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	//shaderUtil.Delete();
}
void Draw() {
	// render
	// ------
	Render3D();
	if (DrawControls) 
	{
		DrawAllControls();
	}
}
void Render3D() {
	glm::vec3 EyePos = Position;
	EyePos.z += EyeHeight;
	if (GetBlock(EyePos) == Water) {
		glClearColor(WaterR, WaterG, WaterB, 0);
	}
	else {
		glClearColor(BackGroundR, BackGroundG, BackGroundB, 0);
	}
	//glDepthFunc(GL_ALWAYS);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDepthFunc(GL_LESS);

	float FogDistance2;
	glm::vec3 FogColor;
	if (GetBlock(EyePos) == Water) {
		float WaterDistance = 50.0f;
		if (WaterDistance > MaxDistance)WaterDistance = MaxDistance;
		FogDistance2 = WaterDistance * WaterDistance;//squared
		FogColor = glm::vec3(WaterR, WaterG, WaterB);
	}
	else
	{
		FogDistance2 = MaxDistance2;
		FogColor = glm::vec3(BackGroundR, BackGroundG, BackGroundB);
	}

#pragma region Update Shaders
	// pass projection matrix to shader (note that in this case it could change every frame)
	projection = glm::perspective(glm::radians(FOV), ScreenProportion//to avoid transforming
		, 0.1f//FLT_EPSILON//0.01f
		, sqrtf(FogDistance2));
	// camera/view transformation
	view = glm::lookAt(EyePos, EyePos + LookDirection, glm::vec3(0, 0, 1));
#pragma endregion
	CropView();//optimisation
#pragma region Draw Out Water
	glBindTexture(GL_TEXTURE_2D, BlockTextures->Location);//binds to active texture
	//set options
	glEnable(GL_CULL_FACE);//checks direction and doesn't draw if direction is not pointing towards camera

	WorldShader->bind();//tell opengl to use worldshader
	WorldShader->setMat4("projection", projection);
	WorldShader->setMat4("view", view);
	WorldShader->setVec3("EyePos", EyePos);
	WorldShader->setFloat("MaxDistance", FogDistance2);
	WorldShader->setVec3("FogColor", FogColor);
	for (int i = 0; i < SaveDiametre2; i++)
	{
		if (ChunkTiles[i] != NULL && IsVisible[i] && ChunkTiles[i]->Populated)
		{
			if (ChunkTiles[i]->Changed)ChunkTiles[i]->GenerateMesh();
			ChunkTiles[i]->DrawCulled();
		}
	}
	glDisable(GL_CULL_FACE);//does not check direction
#pragma region Draw Entities
	if (mobs->size() > 0) {
		DynamicMeshMerger3D->Size = 0;
		for (auto i = mobs->begin(); i != mobs->end(); i++)
		{
			Mob* ActiveMob = *i;
			//Pig* p = (Pig*)ActiveMob;
			//error
			ActiveMob->Render(DynamicMeshMerger3D);
		}
		glBindVertexArray(MobVAO);
		glBindBuffer(GL_ARRAY_BUFFER, MobVBO);
		//reload mesh into gpu
		glBufferData(GL_ARRAY_BUFFER, DynamicMeshMerger3D->Size * sizeof(float)//sizeof(vertices)
			, DynamicMeshMerger3D->MergedArray, GL_DYNAMIC_DRAW);
		glDrawElements(GL_TRIANGLES, DynamicMeshMerger3D->Size / ((3 + 2 + 3) * 4) * 6/*a triangle has 6 vertices*/, GL_UNSIGNED_INT, nullptr);
	}
#pragma endregion
	for (int i = 0; i < SaveDiametre2; i++)
	{
		if (ChunkTiles[i] != NULL && IsVisible[i] && ChunkTiles[i]->Populated)
		{
			ChunkTiles[i]->DrawUnCulled();
		}
	}
	WorldShader->unbind();//unbind worldshader
	glBindTexture(GL_TEXTURE_2D, 0);//unbinds to active texture
	//set options

#pragma endregion
#pragma region Draw Water
	WaterShader->bind();

	WaterShader->setMat4("projection", projection);
	WaterShader->setMat4("view", view);
	WaterShader->setVec3("EyePos", EyePos);
	WaterShader->setFloat("MaxDistance", FogDistance2);
	WaterShader->setVec4("FluidColor", glm::vec4(WaterR, WaterG, WaterB, 0.5));
	WaterShader->setVec3("FogColor", FogColor);

	for (int i = 0; i < SaveDiametre2; i++) 
	{
		if (ChunkTiles[i] != NULL && IsVisible[i] && ChunkTiles[i]->Populated)
			ChunkTiles[i]->DrawFluid();
	}
	WaterShader->unbind();
#pragma endregion
}
Block GetBlock(glm::vec3 pos) {
	return GetBlock(floor(pos.x), floor(pos.y), floor(pos.z));
}
Block GetBlock(int x, int y, int z) {
	const int LoadScaleHor = ChunkScaleHor * SaveDiametre;
	x -= PlusX;
	y -= PlusY;
	if (x < 0 || y < 0 || z < 0 || x >= LoadScaleHor || y >= LoadScaleHor || z >= ChunkScaleVert)
		return Air;
	int ChunkX = x / ChunkScaleHor, ChunkY = y / ChunkScaleHor;
	Chunk* chunk = ChunkTiles[ChunkX + ChunkY * SaveDiametre];
	if (chunk == NULL)
		return Air;
	x = x % ChunkScaleHor;
	y = y % ChunkScaleHor;
	BlockIndex index = x + y * ChunkScaleHor + z * ChunkScale2;
	Block block = *(chunk->data + index);
	return block;
}
glm::vec3 GetLightLevel(int x, int y, int z) 
{
	const int LoadScaleHor = ChunkScaleHor * SaveDiametre;
	x -= PlusX;
	y -= PlusY;
	if (x < 0 || y < 0 || z < 0 || x >= LoadScaleHor || y >= LoadScaleHor || z >= ChunkScaleVert)return glm::vec3(1);
	int ChunkX = x / ChunkScaleHor, ChunkY = y / ChunkScaleHor;
	Chunk* chunk = ChunkTiles[ChunkX + ChunkY * SaveDiametre];
	if (chunk == NULL)return glm::vec3(1);
	x = x % ChunkScaleHor;
	y = y % ChunkScaleHor;
	//z = z + MarginZBottom;
	BlockIndex index = x + (y * StrideY) + (z * StrideZ);
	float brightness = *(chunk->LightMap + index) * BrightnessValueToFloat;
	return glm::vec3(brightness);
}
void SetBlockRange(int x0, int y0, int z0, int x1, int y1, int z1, Block value, const bool addaction = false) 
{
	for (int k = z0; k < z1; k++) 
	{
		for (int j = y0; j < y1; j++)
		{
			for (int i = x0; i < x1; i++)
			{
				SetBlock(i, j, k, value, addaction);
			}
		}
	}
}
void SetBlock(int x, int y, int z, Block block, const bool addaction = false) {
	const int LoadScaleHor = ChunkScaleHor * SaveDiametre;
	x -= PlusX;
	y -= PlusY;
	if (x < 0 || y < 0 || z < 0 || x >= LoadScaleHor || y >= LoadScaleHor || z >= ChunkScaleVert)return;
	int ChunkX = x / ChunkScaleHor, ChunkY = y / ChunkScaleHor;
	int ChunkIndex = ChunkX + ChunkY * SaveDiametre;
	Chunk* chunk = ChunkTiles[ChunkIndex];
	if (chunk == NULL)return;
	//x in chunk
	x %= ChunkScaleHor;
	y %= ChunkScaleHor;
	if (x == 0) {
		Chunk* mx = ChunkTiles[ChunkIndex - 1];
		mx->Changed = true;
	}
	else if (x == ChunkScaleHorMin) 
	{
		Chunk* px = ChunkTiles[ChunkIndex + 1];
		px->Changed = true;
	}
	if (y == 0) {
		Chunk* my = ChunkTiles[ChunkIndex - SaveDiametre];
		my->Changed = true;
	}
	else if (y == ChunkScaleHorMin) {
		Chunk* py = ChunkTiles[ChunkIndex + SaveDiametre];
		py->Changed = true;
	}
	BlockIndex index = x + y * ChunkScaleHor + z * ChunkScale2;
	if (addaction) 
	{
		AddAction(chunk->xPos, chunk->yPos, index, block);
	}
	*(chunk->data + index) = block;
	chunk->Changed = true;
}
bool releasedG = true;
bool flying = true;//indicates wether player has nothing below the feet
bool FluidFeet = false;//indicates wether player has a fluid below the feet
bool lastflying = true;
bool sneak = false;
//const float GravityForce30Fps = 9.8f / 465;//9.8m/s2 with 30 fps, 1 + 2 + 3 + ... 30 = 465
const float GravityForcePerTick = 0.08f;//https://gaming.stackexchange.com/questions/178726/what-is-the-terminal-velocity-of-a-sheep
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
float LastYaw = 0;
void SetActivePath(char* path) 
{
	delete[] ScreenShotPath;
	ScreenShotPath = path;
}
void processInput()
{
	lastflying = flying;
	float RadiansPitch = glm::radians(Pitch);
	float RadiansYaw = glm::radians(-Yaw);
	float CosYaw = cos(RadiansYaw);
	float SinYaw = sin(RadiansYaw);
	float DegreesPerFrame = MSFrame * .2f;
	//if player is on ground:
	if (!flying)
	{
		//rotate speed vector with camera, and substract friction by rotation
		float dYaw = LastYaw - Yaw;
		float friction = pow(0.5f, abs(dYaw / 180));//0.5 speed multiplier when turned 180 degrees
		Speed = glm::rotateZ(Speed, dYaw / 180 * PI) * friction;
	}
	if (glfwGetKey(OutputWindow, GRAVITY_KEY) == GLFW_PRESS) {
		if (releasedG) {
			Gravity = !Gravity;
			releasedG = false;
		}
	}
	else {
		releasedG = true;
	}
	bool Sprintkey = glfwGetKey(OutputWindow, SPRINT_KEY) == GLFW_PRESS;
	if (Sprintkey) {
		FOV = (FOV * 5 + 110) / 6.0f;
	}
	else 
	{
		FOV = (FOV * 5 + 90) / 6.0f;
	}
	float MovementSpeed;
	float step;//the power of this step
	if (Gravity) {
		float Jumpheight = 1.252203f;// vanilla mc: 1.25
		const float Jumpspeed = //0.23f;
			0.42f;//to jump 1.252203 blocks high
		const float SwimUpspeed = GravityForcePerTick + (0.5f * TickTime * (1 -FluidFriction));//0.2f;
		const float standheight = 1.8f;
		height = standheight;

		if (Sprintkey) {
			MovementSpeed = SprintingSpeed;
		}
		else {
			MovementSpeed = WalkingSpeed;
		}
		if (flying)
		{
			if (FluidFeet) {
				bool jump = glfwGetKey(OutputWindow, JUMP_KEY) == GLFW_PRESS;
				if (jump)
				{
					Speed.z += SwimUpspeed;
				}
			}
			step = MovementSpeed * TickTime * (flying ? AirFrictionMultiplier : GroundFrictionMultiplier);
		}
		else
		{
			bool jump = glfwGetKey(OutputWindow, JUMP_KEY) == GLFW_PRESS && CanJump;
			sneak = glfwGetKey(OutputWindow, SNEAK_KEY) == GLFW_PRESS && !jump;
			if (jump)
			{
				Speed.z += Jumpspeed;
				step = 0.18f;//horizontal jump power, .17 is too few
			}
			else {
				if (sneak) {
					sneak = true;
					const float sneakheight = 1.65f;
					height = sneakheight;
					MovementSpeed = SneakingSpeed;
					//Speed.z -= vz;
				}
				step = MovementSpeed * TickTime * (flying ? AirFrictionMultiplier : GroundFrictionMultiplier);
			}
		}
	}
	else {
		if (Sprintkey) {
			MovementSpeed = SprintFlyingSpeed;
		}
		else 
		{
			MovementSpeed = FlyingSpeed;
		}
		step = MovementSpeed * TickTime * AirFrictionMultiplier;
		if (glfwGetKey(OutputWindow, JUMP_KEY) == GLFW_PRESS) {
			Speed.z += step;
		}
		if (glfwGetKey(OutputWindow, SNEAK_KEY) == GLFW_PRESS) {
			Speed.z -= step;
		}

	}
	EyeHeight = height - .18f;
	float vx, vy;
	float TwoKeyMult = 0.70710678118654752440084436210485f;
	vx = CosYaw * step;
	vy = SinYaw * step;
	if (glfwGetKey(OutputWindow, JUMP_TO_BUILDPOS_KEY) == GLFW_PRESS)
	{
		int ContainerCount = GetActionContainerCount();
		if (ContainerCount > 0) {
			int ContainerIndex = rand() % ContainerCount;
			ActionContainer* container = getActionContainerPointer(ContainerIndex);
			std::list<Action*>* actions = container->actions;
			int ActionIndex = rand() % actions->size();
			auto item = actions->begin();
			std::advance(item, ActionIndex);
			Action* action = *item;
			int aIndex = action->index;
			int aX = aIndex % ChunkScaleHor;
			int aY = (aIndex / StrideY) % ChunkScaleHor;
			int aZ = aIndex / StrideZ;
			Position.x = container->posX * ChunkScaleHor + aX;
			Position.y = container->posY * ChunkScaleHor + aY;
			Position.z = aZ;
		}
	}
	if (glfwGetKey(OutputWindow, GUI_KEY) == GLFW_PRESS)
	{
		DrawControls = !DrawControls;
	}
	//https://stackoverflow.com/questions/5844858/how-to-take-screenshot-in-opengl
	if (glfwGetKey(OutputWindow, SCREENSHOT_KEY) == GLFW_PRESS)
	{
		//capture screen
		// Make the BYTE array, factor of 3 because it's RBG.
		BYTE* pixels = new BYTE[4//3 
			* DesktopWidth * DesktopHeight];
		glFinish();//to finish drawing
		glReadPixels(0, 0, DesktopWidth, DesktopHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels);
		ExportImage(DesktopWidth, DesktopHeight, pixels, "screenshot.bmp"//ScreenShotPath
		);//ALWAYS the DOUBLE SLASH
		// Convert to FreeImage format & save to file
		//FIBITMAP* image = FreeImage_ConvertFromRawBits(pixels, DesktopWidth, height, 3 * DesktopHeight, 24, 0x0000FF, 0xFF0000, 0x00FF00, false);
		//FreeImage_Save(FIF_BMP, image, "C:/test.bmp", 0);

		// Free resources
		//FreeImage_Unload(image);
		delete[] pixels;
	}
	//glm::vec3 speed
	/*
	//jump to random build places
	if (glfwGetKey(OutputWindow, JUMP_TO_BUILDPOS_KEY) == GLFW_PRESS)
	{
		string sx = to_string(Position.x);
		string sy = to_string(Position.y);
		string sz = to_string(Position.z);
		MessageBoxA(NULL, "X: " + sx.c_str() + " Y: " + sy.c_str() + " Z: " + sz.c_str(), "WorldCraft", MB_OK);
	}*/
	if (glfwGetKey(OutputWindow, EXIT_KEY) == GLFW_PRESS)
	{
		close = true;
	}
	bool left = glfwGetKey(OutputWindow, LEFT_KEY) == GLFW_PRESS;
	bool right = glfwGetKey(OutputWindow, RIGHT_KEY) == GLFW_PRESS;
	bool forward = glfwGetKey(OutputWindow, FORWARD_KEY) == GLFW_PRESS;
	bool backward = glfwGetKey(OutputWindow, BACKWARD_KEY) == GLFW_PRESS;
	if (left ^ right && forward ^ backward) //else the player can cheat walking diagonal
	{
		vx *= TwoKeyMult;
		vy *= TwoKeyMult;
	}
	if (forward) {
		Speed.y += vy;
		Speed.x += vx;
	}
	if (backward) {
		Speed.y -= vy;
		Speed.x -= vx;
	}
	if (left) {
		Speed.y += vx;
		Speed.x -= vy;
	}
	if (right) {
		Speed.y -= vx;
		Speed.x += vy;
	}
	if (glfwGetKey(OutputWindow, LOOK_LEFT_KEY) == GLFW_PRESS) {
		Yaw -= DegreesPerFrame;
	}
	if (glfwGetKey(OutputWindow, LOOK_RIGHT_KEY) == GLFW_PRESS) {
		Yaw += DegreesPerFrame;
	}
	if (glfwGetKey(OutputWindow, LOOK_UP_KEY) == GLFW_PRESS) {
		Pitch += DegreesPerFrame;
	}
	if (glfwGetKey(OutputWindow, LOOK_DOWN_KEY) == GLFW_PRESS) {
		Pitch -= DegreesPerFrame;
	}
	if (glfwGetKey(OutputWindow, BRAKE_KEY) == GLFW_PRESS) {
		Speed = glm::vec3();
	}
	if (!ClickedLastFrame && glfwGetKey(OutputWindow, DIG_KEY) == GLFW_PRESS) {
		Dig();
	}
	if (!ClickedLastFrame && glfwGetKey(OutputWindow, PLACE_KEY) == GLFW_PRESS) {
		Place();
	}
	Physics();
	LastYaw = Yaw;
}
constexpr float HalfHitboxWidth = PlayerWidth * .5f;
bool CollideHitbox(glm::vec3 v) 
{

	int FromX = floor(v.x - HalfHitboxWidth);
	int FromY = floor(v.y - HalfHitboxWidth);
	int ToX = floor(v.x + HalfHitboxWidth);
	int ToY = floor(v.y + HalfHitboxWidth);
	int FromZ = floor(v.z);
	int ToZ = floor(v.z + height);
	//check hitbox
	for (int pZ = FromZ; pZ <= ToZ; pZ++)
	{
		for (int pY = FromY; pY <= ToY; pY++)
		{
			for (int pX = FromX; pX <= ToX; pX++)
			{
				Block block = GetBlock(pX, pY, pZ);
				if (IsSolidBlock[block])
					return true;
			}
		}
	}
	return false;
}
void Physics() {
	if (Gravity)
	{
		glm::vec3 NewPosition = Position + Speed;
		//check hitbox
		float BounceMultiplier = 0.0f;
		bool xyz = CollideHitbox(NewPosition);
		bool xy = CollideHitbox(glm::vec3(NewPosition.x, NewPosition.y, Position.z));
		bool xz = CollideHitbox(glm::vec3(NewPosition.x, Position.y, NewPosition.z));
		bool yz = CollideHitbox(glm::vec3(Position.x, NewPosition.y, NewPosition.z));
		bool cx = CollideHitbox(glm::vec3(NewPosition.x, Position.y, Position.z));
		bool cy = CollideHitbox(glm::vec3(Position.x, NewPosition.y, Position.z));
		bool cz = CollideHitbox(glm::vec3(Position.x, Position.y, NewPosition.z));
		flying = true;
		CanJump = true;
		if (xyz)//at least one axis collided
		{
			if (!xy)//z collided
			{
				Speed.x *= 0.7f;
				Speed.y *= 0.7f;
				if (!xz || !yz) //corner
				{
					Speed = glm::vec3();
				}
				else
				{
					if(Speed.z<0)//margin 0
					{
						int fz = floor(Position.z);
						Position.z = Position.z - fz + 0.001f < 0 ? fz - 1 : fz;
						flying = false;//hit ground
						BounceMultiplier = GetBounceMultiplier[GetBlock(floor(Position.x), floor(Position.y), floor(Position.z - 1))];
						if (BounceMultiplier > 0 && glfwGetKey(OutputWindow, JUMP_KEY) == GLFW_PRESS) 
						{
							BounceMultiplier = (1 + BounceMultiplier) / 2;
							CanJump = false;
						}
					}
					Speed.z *= BounceMultiplier;
				}
			}
			else if (!xz)//y collided
			{
				if (!yz) //corner
				{
					Speed = glm::vec3();
				}
				else
				{
					Speed.y *= BounceMultiplier;
				}
			}
			else if (!yz)//x collided
			{
				Speed.x *= BounceMultiplier;
			}
			else if (!cx) //y and z collided
			{
				Speed.y *= BounceMultiplier;
				Speed.z *= BounceMultiplier;
			}
			else if (!cy) //x and z collided
			{
				Speed.x *= BounceMultiplier;
				Speed.z *= BounceMultiplier;
			}
			else if (!cz) //x and y collided
			{
				Speed.x *= BounceMultiplier;
				Speed.y *= BounceMultiplier;
			}
			else //x, y and z collided
			{
				Speed *= BounceMultiplier;
			}
		}
		NewPosition = Position + Speed;
		int BlockUnderZ = floor(NewPosition.z - 0.001f);
		int FromX = floor(NewPosition.x - HalfHitboxWidth);
		int FromY = floor(NewPosition.y - HalfHitboxWidth);
		int ToX = floor(NewPosition.x + HalfHitboxWidth);
		int ToY = floor(NewPosition.y + HalfHitboxWidth);
		FluidFeet = false;
		for (int pY = FromY; pY <= ToY; pY++)
		{
			for (int pX = FromX; pX <= ToX; pX++)
			{
				Block block = GetBlock(pX, pY, BlockUnderZ);
				bool solid = IsSolidBlock[block];
				if (solid)
				{
					flying = false;
					FluidFeet = false;
					goto BlockUnder;
				}
				else if (GetBlockType[block] == Fluid)
				{
					FluidFeet = true;
				}
			}
		}
		if (flying && sneak && !lastflying)
		{
			Speed = glm::vec3();//prevent from going off the edge
			flying = false;
		}
	BlockUnder:
		Position += Speed;
		if (flying) {
			Speed.z -= GravityForcePerTick;//pow(gravitymult, 1.0f / FPS);
		}
		Speed *= (flying) ? FluidFeet ? FluidFriction : AirFriction : GroundFriction;
	}
	else {
		Position += Speed;
		Speed *= AirFriction;
	}
}