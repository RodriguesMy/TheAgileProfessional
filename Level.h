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

enum EDoorState
{
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
	IMesh* m_MWall;
	IMesh* m_MDoor;
	IMesh* m_MPillars;
	IMesh* m_MPedestal;
	IMesh* m_MKey;
	IMesh* m_MCoin;
	string m_KeyData; //used to recreate the key when you want to reload the key(meaning it was deleted and we want it back)
	Vector m_PlayerSPos;
	Vector m_Min; // holds the coordinates of the bottom left corner of the level(with y being 0)
	Vector m_Max; // holds the coordinates of the top right corner of the level(with y being 0)
	int m_CoinCount;
public:
	CLevel(I3DEngine* myEngine);
	~CLevel();
	void NextLevel(vector<WallStruct>& Walls,vector<DoorStruct>& Doors,vector<PillarStruct>& Pillars,IModel*& Key, CGuard& Guard, vector<IModel*>& Coins);
	void RemoveKey(IModel*& Key);
	void RemoveCoin(IModel*& Coin);
	int GetLevelNumber(){return m_LevelIt;}
	Vector GetMin() { return m_Min; }
	Vector GetPlayerSPos(){return m_PlayerSPos;}
	int GetNumberOfCoins() { return m_CoinCount; }
	void ReloadKey(IModel*& Key);
	void Restart(vector<WallStruct>& Walls, vector<DoorStruct>& Doors, vector<PillarStruct>& Pillars, IModel*& Key, CGuard& Guard, vector<IModel*>& Coins);
private:
	bool IncreaseLevelIt();
	IModel* CreateModel(IMesh* mesh, string data, bool Check, float* scale = 0, float* rot = 0);//The check parameter is to alter the max and min or not
	void ClearLevel(vector<WallStruct>& Walls, vector<DoorStruct>& Doors,vector<PillarStruct>& Pillars,IModel*& Key, CGuard& Guard, vector<IModel*>& Coins);
	void CreateGrid(vector<WallStruct> Walls, vector<PillarStruct> Pillars, CGuard& Guard);
};
