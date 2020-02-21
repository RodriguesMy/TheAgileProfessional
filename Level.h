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
	int m_MapIt;
	IMesh* m_MWall;
	IMesh* m_MDoor;
	IMesh* m_MMainDoor;
public:
	CLevel(I3DEngine* myEngine);
	~CLevel();
	bool NextLevel(vector<IModel*> Walls,vector<IModel*> Doors, IModel* MainDoor);
};