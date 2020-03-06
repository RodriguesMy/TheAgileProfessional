#include "Level.h"
#include <iostream>
#include <algorithm>
#include <string>

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
	IModel* output = mesh->CreateModel();
	float fdata[5] = { 0.0f,0.0f,0.0f,0.0f,1.0f };
	for (int i = 0; i < 5; i++) {
		if (data.find(",")!=string::npos) {
			int pos = data.find(",");
			fdata[i] = stof(data.substr(0, pos));
			data = data.substr(pos+1);
		}
		else
			break;
	}
	output->Scale(fdata[4]);
	output->SetPosition(fdata[0], fdata[1], fdata[2]);
	output->RotateY(fdata[3]);
	return output;
}

void CLevel::ClearLevel(vector<IModel*> Walls, vector<IModel*> Doors, IModel* MainDoor) {
	while(!Walls.empty()){
		m_MWall->RemoveModel(Walls.back());
		Walls.pop_back();
	}
	while (!Doors.empty()) {
		m_MDoor->RemoveModel(Doors.back());
		Doors.pop_back();
	}
	m_MDoor->RemoveModel(MainDoor);
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
				if (!isalpha(input[0])) {
					switch (Current){
					case wall:
						Walls.push_back(CreateModel(m_MWall,input));
						break;
					case door:
						Doors.push_back(CreateModel(m_MDoor, input));
						break;
					case maindoor:
						MainDoor = CreateModel(m_MDoor, input);
						break;
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
					else
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