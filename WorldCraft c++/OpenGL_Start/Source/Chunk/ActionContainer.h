#include "WorldConstants.h"
#include <stdio.h>
#include <list>
#pragma once

struct  Action
{
public:
	Action(BlockIndex index, Block block);
	~Action();
	BlockIndex index;
	Block block;
private:

};
//stores all positions from blocks which player has changed
struct ActionContainer
{
public:
	int posX, posY;
	//ChunkActionContainer();
	ActionContainer(int posX, int posY);
	ActionContainer(int posX, int posY, Action* FirstAction);
	std::list<Action*>* actions;
private:

};
void ClearActionContainers();
int GetActionContainerCount();
ActionContainer* GetContainer(int posX, int posY);
void AddAction(int PosX, int Posy, BlockIndex index, Block block);
ActionContainer* getActionContainerPointer(int index);