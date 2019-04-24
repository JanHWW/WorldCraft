//memory leak detection
#define _CRTDBG_MAP_ALLOC 
#include <stdlib.h> 
#include <crtdbg.h>
#include <Windows.h>

//#include <iostream>
//#include <filesystem>
#include "Rendering/RenderWindow.h"
#include "main.h"

int main()
{
	if (Initialize()) {
		string line;
		while (true)
		{
			getline(cin, line);
			if (line == "play")
			{
				if (Run())
					break;
			}
			else if (line == "exit")
			{
				break;
			}
			else if (line == "delete chunks")
			{
				DeleteChunks();
			}
			else {
				const int MAX_WORDS = 4;
				string words[MAX_WORDS];
				int i = 0;
				stringstream ssin(line);
				while (ssin.good() && i < 4) {
					ssin >> words[i];
					++i;
				}
				if (words[0] == "open")
				{
					string filename = line.substr(5);//'open ' = 5 chars
					Open(filename.c_str());
				}
				if (words[0] == "screenshotpath")
				{
					SetActivePath((char*)words[1].c_str());
					//delete[] ScreenShotPath;
					//ScreenShotPath = (char*)words[1].c_str();
				}
				else if (words[0] == "power")
				{
					Power = stoi(words[1]);
				}
				else if (words[0] == "mine")
				{
					MinE = strtof(words[1].c_str(), 0);
					InitializeTerrain();
				}
				else if (words[0] == "maxe")
				{
					MaxE = strtof(words[1].c_str(), 0);
					InitializeTerrain();
				}
				else if (words[0] == "minm")
				{
					MinM = strtof(words[1].c_str(), 0);
					InitializeTerrain();
				}
				else if (words[0] == "maxm")
				{
					MaxM = strtof(words[1].c_str(), 0);
					InitializeTerrain();
				}
				else if (words[0] == "seed")
				{
					Seed = stoi(words[1]);
					InitializeTerrain();
				}
				else if (words[0] == "save")//'save ' = 5 chars
				{
					string filename = line.substr(5);
					Save(filename.c_str());
				}
				else if (words[0] == "mousesensitivity")
				{
					MouseSensitivity = strtof(words[1].c_str(), 0);
				}
				else if (words[0] == "backgroundcolor")
				{
					BackGroundR = strtof(words[1].c_str(), 0);
					BackGroundG = strtof(words[2].c_str(), 0);
					BackGroundB = strtof(words[3].c_str(), 0);
				}
				else if (words[0] == "watercolor")
				{
					WaterR = strtof(words[1].c_str(), 0);
					WaterG = strtof(words[2].c_str(), 0);
					WaterB = strtof(words[3].c_str(), 0);
				}
				else
				{
					cout << "action is not recognised" << endl;
				}
			}
		}
	}
	Delete();
	_CrtDumpMemoryLeaks();
	return 0;
}
bool Initialize()
{
	if (!glfwInit())
		return false;
	GetDesktopResolution(DesktopWidth, DesktopHeight);
	Seed = time(NULL);
	Power = 5;
	MinE = 0;
	MaxE = 1;
	MinM = 0;
	MaxM = 1;
	InitializeTerrain();
	if (!CreateRenderWindow(DesktopWidth, DesktopHeight, "WorldCraft 3.14")) return false;
	return true;
}