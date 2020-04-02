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
	float doorXLengthArea;
	float doorYLengthArea;
	float doorZLengthArea;
	float doorMovementSpeed;
};

struct WallStruct {
	IModel* model;
	float rot;
	float wallXLength;
	float wallYLength;
	float wallZLength;
};

struct PillarStruct {
	IModel* model;
	EPillarType type;
	float pillarXLength;
	float pillarYLength;
	float pillarZLength;
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
	IModel* m_Key;
	Vector m_PlayerSPos;
public:
	CLevel(I3DEngine* myEngine);
	~CLevel();
	bool NextLevel(vector<WallStruct>& Walls,vector<DoorStruct>& Doors,vector<PillarStruct>& Pillars,IModel*& Key);
	int GetLevelNumber() { return m_LevelIt; }
	void RemoveKey();
	IModel* getKey();
	void SetUpKey();
	void UpdateKey(float keyMovingSpeed, float dt,bool keyFound);
	Vector GetPlayerSPos(){return m_PlayerSPos;}
private:
	bool IncreaseLevelIt();
	IModel* CreateModel(IMesh* mesh, string data, float& rot);
	IModel* CreateModel(IMesh* mesh, string data);
	void ClearLevel(vector<WallStruct>& Walls, vector<DoorStruct>& Doors,vector<PillarStruct>& Pillars,IModel*& Key);
};
