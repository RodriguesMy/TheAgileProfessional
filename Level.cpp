#include "Level.h"
#include <iostream>
#include <algorithm>
#include <string>

CLevel::CLevel(I3DEngine* myEngine)
{
	m_LevelIt = -1;
	m_MDoor = myEngine->LoadMesh("Door.x");
	m_MWall = myEngine->LoadMesh("wall.x");
	m_MPillars = myEngine->LoadMesh("Pillar.x");
	m_MPedestal = myEngine->LoadMesh("Pedestal.x");
	m_MKey = myEngine->LoadMesh("Key.x");
}

CLevel::~CLevel()
{
}

IModel* CLevel::CreateModel(IMesh* mesh,string data,float rot) {
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
	rot = fdata[3];
	output->RotateY(fdata[3]);
	return output;
}

void CLevel::ClearLevel(vector<WallStruct>& Walls, vector<DoorStruct>& Doors,vector<IModel*>& Pillars,IModel*& Key) {
	while(!Walls.empty()){
		m_MWall->RemoveModel(Walls.back().model);
		Walls.pop_back();
	}
	while (!Doors.empty()) {
		m_MDoor->RemoveModel(Doors.back().model);
		Doors.pop_back();
	}
	while (!Pillars.empty()) {
		m_MPillars->RemoveModel(Pillars.back());
	}
	if (Key != NULL) {
		m_MKey->RemoveModel(Key);
		Key = NULL;
	}
}

bool CLevel::NextLevel(vector<WallStruct>& Walls, vector<DoorStruct>& Doors,vector<IModel*>& Pillars,IModel*& Key) {
	if (IncreaseLevelIt()) {
		ifstream File("./Level/" + m_Levels[m_LevelIt] + ".txt");
		if (File.is_open()) {
			ClearLevel(Walls, Doors, Pillars,Key);
			enum EModelType {
				wall,
				door,
				endingdoor,
				startingdoor,
				pillar,
				pedestal,
				key
			};
			EModelType Current=wall;
			string input;
			while (getline(File,input))
			{
				input.erase(remove_if(input.begin(), input.end(), isspace));
				if (!isalpha(input[0])) {
					switch (Current) {
					case wall:
						WallStruct wall;
						wall.model = CreateModel(m_MWall, input, wall.rot);
						Walls.push_back(wall);
						break;
					case door:
						DoorStruct Sdoor;
						Sdoor.model = CreateModel(m_MDoor, input);
						Sdoor.state = 3;
						Sdoor.type = simple;
						Doors.push_back(Sdoor);
						break;
					case endingdoor:
						for (int i = 0; i < Doors.size(); i++) {
							if (Doors[i].type = ending) {
								m_MDoor->RemoveModel(Doors.at(i).model);
								Doors.erase(Doors.begin()+i);
							}
						}
						DoorStruct Mdoor;
						Mdoor.model = CreateModel(m_MDoor, input);
						Mdoor.state = 3;
						Mdoor.type = ending;
						Doors.push_back(Mdoor);
						break;
					case startingdoor:
						for (int i = 0; i < Doors.size(); i++) {
							if (Doors[i].type = starting) {
								m_MDoor->RemoveModel(Doors.at(i).model);
								Doors.erase(Doors.begin() + i);
							}
						}
						DoorStruct door;
						door.model = CreateModel(m_MDoor, input);
						door.state = 3;
						door.type = starting;
						Doors.push_back(door);
						break;
					case pillar:
						Pillars.push_back(CreateModel(m_MPillars, input));
						break;
					case pedestal:
						Pillars.push_back(CreateModel(m_MPedestal, input));
						break;
					case key:
						if (Key != NULL)
							m_MKey->RemoveModel(Key);
						Key = CreateModel(m_MKey, input);
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
					else if (input == "pillar")
						Current = pillar;
					else if (input == "pedestal")
						Current = pedestal;
					else if (input == "endingdoor")
						Current = endingdoor;
					else if (input == "startingdoor")
						Current = startdoor;
					else if (input == "key")
						Current = key;
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
	if (m_LevelIt < m_Levels.size())
		return true;
	else
		return false;
}
