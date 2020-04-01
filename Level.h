#pragma once

#include <TL-Engine.h>
#include <vector>
#include <fstream>
using namespace std;
using namespace tle;

enum EDoortype {
	simple,
	starting,
	ending
};

struct DoorStruct {
	IModel* model;
	int state;
	EDoortype type;
};

struct WallStruct {
	IModel* model;
	float rot;
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
public:
	CLevel(I3DEngine* myEngine);
	~CLevel();
	bool NextLevel(vector<WallStruct>& Walls,vector<DoorStruct>& Doors,vector<IModel*>& Pillars,IModel*& Key);
	int GetLevelNumber() { return m_LevelIt; }
private:
	bool IncreaseLevelIt();
	IModel* CreateModel(IMesh* mesh, string data, float rot = 0);
	void ClearLevel(vector<WallStruct>& Walls, vector<DoorStruct>& Doors,vector<IModel*>& Pillars,IModel*& Key);
};