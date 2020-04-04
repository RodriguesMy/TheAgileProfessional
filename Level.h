#pragma once

#include <TL-Engine.h>
#include <vector>
#include <fstream>
using namespace std;
using namespace tle;

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
struct Vector{
	float x;
	float y;
	float z;
};
struct DoorStruct {
	IModel* model;
	EDoorState state;
	EDoortype type;
	Vector length;
	float movementSpeed;
	Vector areaLength;
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
		"Level3"
	};
	int m_LevelIt;
	IMesh* m_MWall;
	IMesh* m_MDoor;
	IMesh* m_MPillars;
	IMesh* m_MPedestal;
	IMesh* m_MKey;
	Vector m_PlayerSPos;
public:
	CLevel(I3DEngine* myEngine);
	~CLevel();
	bool NextLevel(vector<WallStruct>& Walls,vector<DoorStruct>& Doors,vector<PillarStruct>& Pillars,IModel*& Key);
	int GetLevelNumber() { return m_LevelIt; }
	void RemoveKey(IModel*& Key);
	Vector GetPlayerSPos(){return m_PlayerSPos;}
private:
	bool IncreaseLevelIt();
	IModel* CreateModel(IMesh* mesh, string data, float* scale = 0, float* rot = 0);
	void ClearLevel(vector<WallStruct>& Walls, vector<DoorStruct>& Doors,vector<PillarStruct>& Pillars,IModel*& Key);
};
