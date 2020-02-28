#include "Level.h"
#include <fstream>
#include <iostream>
#include <algorithm>
using namespace std;


CLevel::CLevel(I3DEngine* myEngine)
{
	m_LevelIt = -1;
	m_MDoor = myEngine->LoadMesh("");
	m_MWall = myEngine->LoadMesh("");
}


CLevel::~CLevel()
{
}

IModel* CLevel::CreateModel(IMesh* mesh,string data) {
	//vector
	while (data.find(",")) {
		int pos = data.find(",");
		float currentdata = stof(data.substr(0, pos));
		//push to vector
	}
	//read from vector to create model
}

bool CLevel::NextLevel(vector<IModel*> Walls, vector<IModel*> Doors, IModel* MainDoor) {
	if (IncreaseLevelIt()) {
		ifstream File("./Level/" + m_Levels[m_LevelIt] + ".txt");
		if (File.is_open()) {
			enum EModelType {
				wall,
				door,
				maindoor
			};
			EModelType Current=wall;
			string input;
			while (getline(File,input))
			{
				input.erase(remove_if(input.begin(), input.end(), isspace));
				if (isdigit(input[0])) {

				}
				else {
					switch (Current)
					{
					case wall:
						Walls.push_back(CreateModel(m_MWall,input));
						break;
					case door:
						break;
					case maindoor:
						break;
					}
				}
			}
			return true;
		}
		else {
			cout << "File not found" << endl;
		}
	}
	else {
		return false;
	}
}

bool CLevel::IncreaseLevelIt() {
	m_LevelIt++;
	if (m_LevelIt >= m_Levels->size())
		return false;
	else
		return true;
}