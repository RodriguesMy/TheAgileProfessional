#pragma once

#include <fstream>
#include "Definitions.h"
#include "CGuard.h"

enum EPillarType {
	typePillar,
	typePedestal
};

enum EDoortype {
	simple,
	starting,
	ending
};

enum EDoorState {
	DOOR_OPENING,
	DOOR_CLOSING,
	DOOR_CLOSED,
	DOOR_OPEN
};

struct DoorStruct {
	IModel* model;
	EDoorState state;
	EDoortype type;
	Vector length;
	float movementSpeed;
	Vector areaLength;
	float MaxDoorLimit;
	float CurrentDoorLimit;
};

struct WallStruct {
	IModel* model;
	Vector length;
};

struct PillarStruct {
	IModel* model;
	EPillarType type;
	Vector length;
};

class CLevel
{
private:
	vector<string> m_Levels = {
		"Level1",
		"Level2",
		"Level3",
		"Level4"
	};
	int m_LevelIt;
	//Meshes
	IMesh* m_MWall;
	IMesh* m_MDoor;
	IMesh* m_MPillars;
	IMesh* m_MPedestal;
	IMesh* m_MKey;
	IMesh* m_MCoin;

	string m_KeyData; //used to recreate the key when you want to reload the key(meaning it was deleted and we want it back)
	Vector m_PlayerSPos; //Players starting position
	Vector m_Min; // holds the coordinates of the bottom left corner of the level(with y being 0)
	Vector m_Max; // holds the coordinates of the top right corner of the level(with y being 0)
	int m_CoinCount;
public:
	CLevel(I3DEngine* myEngine);
	~CLevel();
	void NextLevel(vector<WallStruct>& Walls,vector<DoorStruct>& Doors,vector<PillarStruct>& Pillars,IModel*& Key, CGuard& Guard, vector<IModel*>& Coins); //Creates the next level
	void RemoveKey(IModel*& Key);
	void RemoveCoin(IModel*& Coin);
	void ReloadKey(IModel*& Key); //if the key is deleted recreate it
	void Restart(vector<WallStruct>& Walls, vector<DoorStruct>& Doors, vector<PillarStruct>& Pillars, IModel*& Key, CGuard& Guard, vector<IModel*>& Coins);
	//Getters
	int GetLevelNumber(){return m_LevelIt;}
	Vector GetMin() { return m_Min; }
	Vector GetPlayerSPos(){return m_PlayerSPos;}
	int GetNumberOfCoins() { return m_CoinCount; }

private:
	bool IncreaseLevelIt(); // increases the level iterator and if it doesnt excite the number of levels then it returns true otherwise false
	IModel* CreateModel(IMesh* mesh, string data, bool Check, float* scale = 0, float* rot = 0); //creates and returns a models acording to the mesh and data provided, the check variable is to show if this model should affect the max and min
	void ClearLevel(vector<WallStruct>& Walls, vector<DoorStruct>& Doors,vector<PillarStruct>& Pillars,IModel*& Key, CGuard& Guard, vector<IModel*>& Coins);
	void CreateGrid(vector<WallStruct> Walls, vector<PillarStruct> Pillars, CGuard& Guard); // creates a grid where 1 is not obstacle and 0 being there is an obstacle there. This grid has 1 to 5 scale
};
