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
	ending,
	exiting
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
	Vector() {
		x = 0;
		y = 0;
		z = 0;
	}
	Vector(float X, float Y, float Z) {
		this->x = X;
		this->y = Y;
		this->z = Z;
	}
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
	string m_KeyData; //used to recreate the key when you want to reload the key(meaning it was deleted and we want it back)
	Vector m_PlayerSPos;
	Vector m_Min; // holds the coordinates of the bottom left corner of the level(with y being 0)
	Vector m_Max; // holds the coordinates of the top right corner of the level(with y being 0)
	vector<vector<int>> Grid; // A top side view of the entire level scaled so each element of the 2d dynamic array repressents 5 units
public:
	CLevel(I3DEngine* myEngine);
	~CLevel();
	bool NextLevel(vector<WallStruct>& Walls,vector<DoorStruct>& Doors,vector<PillarStruct>& Pillars,IModel*& Key);
	void RemoveKey(IModel*& Key);
	int GetLevelNumber(){return m_LevelIt;}
	Vector GetPlayerSPos(){return m_PlayerSPos;}
	void ReloadKey(IModel*& Key);
	void Restart(vector<WallStruct>& Walls, vector<DoorStruct>& Doors, vector<PillarStruct>& Pillars, IModel*& Key);
private:
	bool IncreaseLevelIt();
	IModel* CreateModel(IMesh* mesh, string data, bool Check, float* scale = 0, float* rot = 0);//The check parameter is to alter the max and min or not
	void ClearLevel(vector<WallStruct>& Walls, vector<DoorStruct>& Doors,vector<PillarStruct>& Pillars,IModel*& Key);
	void CreateGrid(vector<WallStruct> Walls, vector<PillarStruct> Pillars);
};
