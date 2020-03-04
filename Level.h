#pragma once

#include <TL-Engine.h>
#include <vector>
#include <fstream>
using namespace std;
using namespace tle;

class CLevel
{
private:
	string m_Levels[3] = {
		"",
		"",
		""
	};
	int m_LevelIt;
	IMesh* m_MWall;
	IMesh* m_MDoor;
public:
	CLevel(I3DEngine* myEngine);
	~CLevel();
	bool NextLevel(vector<IModel*> Walls,vector<IModel*> Doors, IModel* MainDoor);
private:
	bool IncreaseLevelIt();
	IModel* CreateModel(IMesh* mesh,string data);
	void ClearLevel(vector<IModel*> Walls, vector<IModel*> Doors, IModel* MainDoor);
};