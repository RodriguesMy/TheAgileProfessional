#include "Level.h"
#include <iostream>
#include <algorithm>
#include <string>

CLevel::CLevel(I3DEngine* myEngine)
{
	m_LevelIt = -1;
	m_MDoor = myEngine->LoadMesh("10057_wooden_door_v3_iterations-2.x");
	m_MWall = myEngine->LoadMesh("wall.x");
	//m_MPillars = myEngine->LoadMesh("pillar.x");
}


CLevel::~CLevel()
{
}

IModel* CLevel::CreateModel(IMesh* mesh,string data) {
	IModel* output = mesh->CreateModel();
	float fdata[5] = { 0.0f,0.0f,0.0f,0.0f,1.0f };
	for (int i = 0; i < 5; i++) {
		int pos = data.find(",");
		fdata[i] = stof(data.substr(0, pos));
		if (data.find(",") == string::npos)
			break;
		data = data.substr(pos + 1);		
	}
	output->Scale(fdata[4]);
	output->SetPosition(fdata[0], fdata[1], fdata[2]);
	output->RotateY(fdata[3]);
	return output;
}

void CLevel::ClearLevel(vector<IModel*> Walls, vector<IModel*> Doors,/*vector<IModel*> Pillars, */IModel* MainDoor) {
	while(!Walls.empty()){
		m_MWall->RemoveModel(Walls.back());
		Walls.pop_back();
	}
	while (!Doors.empty()) {
		m_MDoor->RemoveModel(Doors.back());
		Doors.pop_back();
	}
	if (MainDoor != 0) {
		m_MDoor->RemoveModel(MainDoor);
	}
}

bool CLevel::NextLevel(vector<IModel*> Walls, vector<IModel*> Doors,/*vector<IModel*> Pillars,*/ IModel* MainDoor) {
	if (IncreaseLevelIt()) {
		ifstream File("./Level/" + m_Levels[m_LevelIt] + ".txt");
		if (File.is_open()) {
			ClearLevel(Walls, Doors, /*Pillars,*/ MainDoor);
			enum EModelType {
				wall,
				door,
				maindoor,
				pillar
			};
			EModelType Current=wall;
			string input;
			while (getline(File,input))
			{
				input.erase(remove_if(input.begin(), input.end(), isspace));
				if (!isalpha(input[0])) {
					switch (Current) {
					case wall:
						Walls.push_back(CreateModel(m_MWall, input));
						break;
					case door:
						Doors.push_back(CreateModel(m_MDoor, input));
						break;
					case maindoor:
						MainDoor = CreateModel(m_MDoor, input);
						break;
					/*case pillar:
						Pillars.push_back(CreateModel(m_MPillars, input));
						break;*/
					}
				}
				else {
					for (int i = 0; i < input.size(); i++) {
						input[i] = tolower(input[i]);
					}
					if (input == "wall")
						Current = wall;
					else if (input == "door")
						Current = door;
					/*else if (input == "pillar")
						Current = pillar;*/
					else if (input == "maindoor")
						Current = maindoor;
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