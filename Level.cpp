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

IModel* CLevel::CreateModel(IMesh* mesh,string data,float* scale, float* rot) {
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
	if (rot != 0) {
		*rot = fdata[3];
	}
	if (scale != 0) {
		*scale = fdata[4];
	}
	output->RotateY(fdata[3]);
	return output;
}

void CLevel::ClearLevel(vector<WallStruct>& Walls, vector<DoorStruct>& Doors,vector<PillarStruct>& Pillars,IModel*& Key) {
	while(!Walls.empty()){
		m_MWall->RemoveModel(Walls.back().model);
		Walls.pop_back();
	}
	while (!Doors.empty()) {
		m_MDoor->RemoveModel(Doors.back().model);
		Doors.pop_back();
	}
	while (!Pillars.empty()) {
		if (Pillars.back().type == typePillar) {
			m_MPillars->RemoveModel(Pillars.back().model);
		}
		else if (Pillars.back().type == typePedestal) {
			m_MPedestal->RemoveModel(Pillars.back().model);
		}
		Pillars.pop_back();
	}
	if (Key != NULL) {
		m_MKey->RemoveModel(Key);
		Key = NULL;
	}
	m_PlayerSPos.x = 0;
	m_PlayerSPos.y = 0;
	m_PlayerSPos.z = 0;
}
//STOP
bool CLevel::NextLevel(vector<WallStruct>& Walls, vector<DoorStruct>& Doors,vector<PillarStruct>& Pillars,IModel*& Key) {
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
				key,
				exit
			};
			EModelType Current=wall;
			string input;
			while (getline(File,input))
			{
				input.erase(remove_if(input.begin(), input.end(), isspace));
				if (input[0] == '/'); // if it starts with / then its a comment so they code skips that line
				else if (!isalpha(input[0])) { // if it doesnt start with a letter it must be the information for the model(x,y,z,scale,rot)
					float scale;
					float rotation;
					switch (Current) {
					case wall:
						Walls.push_back(*(new WallStruct));
						Walls.back().model = CreateModel(m_MWall, input, &scale, &rotation);
						Walls.back().length.y = scale * 2;
						if (rotation == 0 || rotation == 180) {
							Walls.back().length.x = scale * 2;
							Walls.back().length.z = 1;
						}
						else if (rotation == 90 || rotation == 270) {
							Walls.back().length.x = 1;
							Walls.back().length.z = scale * 2;
						}
						break;
					case door:
						Doors.push_back(*(new DoorStruct));
						Doors.back().model = CreateModel(m_MDoor, input,&scale,&rotation);
						Doors.back().state = DOOR_CLOSED;
						Doors.back().type = simple;
						Doors.back().movementSpeed = 1.6 * scale;
						Doors.back().areaLength.y = 65 * scale;
						Doors.back().length.y = 100 * scale;
						Doors.back().MaxDoorLimit = 110*scale;
						Doors.back().CurrentDoorLimit = 0;
						if (rotation == 0 || rotation == 180) {
							Doors.back().areaLength.x = 33 * scale;
							Doors.back().areaLength.z = 65 * scale;
							Doors.back().length.x = 13 * scale;
							Doors.back().length.z = 33 * scale;
						}
						else if (rotation == 90 || rotation == 270) {

							Doors.back().areaLength.x = 65 * scale;
							Doors.back().areaLength.z = 33 * scale;
							Doors.back().length.x = 33 * scale;
							Doors.back().length.z = 13 * scale;
						}
						break;
					case endingdoor:
						for (int i = 0; i < Doors.size(); i++) {
							if (Doors[i].type == ending) {
								m_MDoor->RemoveModel(Doors.at(i).model);
								Doors.erase(Doors.begin()+i);
							}
						}
						Doors.push_back(*(new DoorStruct));
						Doors.back().model = CreateModel(m_MDoor, input,&scale,&rotation);
						Doors.back().state = DOOR_CLOSED;
						Doors.back().type = ending;
						Doors.back().movementSpeed = 1.6 * scale;
						Doors.back().areaLength.y = 65 * scale;
						Doors.back().length.y = 85 * scale;
						Doors.back().MaxDoorLimit = 110 * scale;
						Doors.back().CurrentDoorLimit = 0;
						if (rotation == 0 || rotation == 180) {
							Doors.back().areaLength.x = 33 * scale;
							Doors.back().areaLength.z = 65 * scale;
							Doors.back().length.x = 13 * scale;
							Doors.back().length.z = 33 * scale;
						}
						else if (rotation == 90 || rotation == 270) {
							Doors.back().areaLength.x = 65 * scale;
							Doors.back().areaLength.z = 33 * scale;
							Doors.back().length.x = 33 * scale;
							Doors.back().length.z = 13 * scale;
						}
						break;
					case startingdoor:
						for (int i = 0; i < Doors.size(); i++) {
							if (Doors[i].type == starting) {
								m_MDoor->RemoveModel(Doors.at(i).model);
								Doors.erase(Doors.begin() + i);
							}
						}
						Doors.push_back(*(new DoorStruct));
						Doors.back().model = CreateModel(m_MDoor, input,&scale,&rotation);
						Doors.back().state = DOOR_CLOSED;
						Doors.back().type = starting;
						Doors.back().movementSpeed = 1.6*scale;
						Doors.back().areaLength.y = 65 * scale;
						Doors.back().length.y =90 * scale;
						Doors.back().MaxDoorLimit = 110 * scale;
						Doors.back().CurrentDoorLimit = 0;
						if (rotation == 0 || rotation == 180) {
							Doors.back().areaLength.x = 33 * scale;
							Doors.back().areaLength.z = 65 * scale;
							Doors.back().length.x = 13 * scale;
							Doors.back().length.z = 33 * scale;
						}
						else if (rotation == 90 || rotation == 270) {
							Doors.back().areaLength.x = 65 * scale;
							Doors.back().areaLength.z = 33 * scale;
							Doors.back().length.x = 33 * scale;
							Doors.back().length.z = 13 * scale;
						}
						m_PlayerSPos.x = Doors.back().model->GetX();
						m_PlayerSPos.y = Doors.back().model->GetY();
						m_PlayerSPos.z = Doors.back().model->GetZ() - 20;
						break;
					case exit:
						Doors.push_back(*(new DoorStruct));
						Doors.back().model = CreateModel(m_MDoor, input, &scale, &rotation);
						Doors.back().state = DOOR_CLOSED;
						Doors.back().type = exiting;
						Doors.back().movementSpeed = 1.6 * scale;
						Doors.back().areaLength.y = 65 * scale;
						Doors.back().length.y = 90 * scale;
						Doors.back().MaxDoorLimit = 110 * scale;
						Doors.back().CurrentDoorLimit = 0;
						if (rotation == 0 || rotation == 180) {
							Doors.back().areaLength.x = 33 * scale;
							Doors.back().areaLength.z = 65 * scale;
							Doors.back().length.x = 13 * scale;
							Doors.back().length.z = 33 * scale;
						}
						else if (rotation == 90 || rotation == 270) {
							Doors.back().areaLength.x = 65 * scale;
							Doors.back().areaLength.z = 33 * scale;
							Doors.back().length.x = 33 * scale;
							Doors.back().length.z = 13 * scale;
						}
						m_PlayerSPos.x = Doors.back().model->GetX();
						m_PlayerSPos.y = Doors.back().model->GetY();
						m_PlayerSPos.z = Doors.back().model->GetZ() - 20;
						break;
					case pillar:
						Pillars.push_back(*(new PillarStruct));
						Pillars.back().model = CreateModel(m_MPillars, input,&scale);
						Pillars.back().type = typePillar;
						Pillars.back().length.x = 0.5*scale;
						Pillars.back().length.y = 5*scale;
						Pillars.back().length.z = 0.5*scale;
						break;
					case pedestal:
						Pillars.push_back(*(new PillarStruct));
						Pillars.back().model = CreateModel(m_MPedestal, input,&scale);
						Pillars.back().type = typePedestal;
						Pillars.back().length.x = 2 * scale;
						Pillars.back().length.y = 5 * scale;;
						Pillars.back().length.z = 2 * scale;
						break;
					case key:
						if (Key != NULL)
							m_MKey->RemoveModel(Key);
						Key = CreateModel(m_MKey, input);
						Key->RotateX(90);
					}
				}
				else {//if it starts with a character then change
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
						Current = startingdoor;
					else if (input == "exit")
						Current = exit;
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

void CLevel::RemoveKey(IModel*& Key) {
	m_MKey->RemoveModel(Key);
	Key = NULL;
}
