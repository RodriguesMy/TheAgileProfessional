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
}

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
				key
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
						WallStruct wall;
						wall.model = CreateModel(m_MWall, input, &scale, &rotation);
						wall.length.y = scale * 2;
						if (rotation == 0 || rotation == 180) {
							wall.length.x = scale * 2;
							wall.length.z = 1;
						}
						else if (rotation == 90 || rotation == 270) {
							wall.length.x = 1;
							wall.length.z = scale * 2;
						}
						Walls.push_back(wall);
						break;
					case door:
						DoorStruct Sdoor;
						Sdoor.model = CreateModel(m_MDoor, input,&scale,&rotation);
						Sdoor.state = DOOR_CLOSED;
						Sdoor.type = simple;
						Sdoor.movementSpeed = 0.25;
						Sdoor.areaLength.y = 65 * scale;
						Sdoor.length.y = 100 * scale;
						Sdoor.MaxDoorLimit = 16.5;
						Sdoor.CurrentDoorLimit = 0;
						if (rotation == 0 || rotation == 180) {
							Sdoor.areaLength.x = 33 * scale;
							Sdoor.areaLength.z = 65 * scale;
							Sdoor.length.x = 13 * scale;
							Sdoor.length.z = 33 * scale;
						}
						else if (rotation == 90 || rotation == 270) {

							Sdoor.areaLength.x = 65 * scale;
							Sdoor.areaLength.z = 33 * scale;
							Sdoor.length.x = 33 * scale;
							Sdoor.length.z = 13 * scale;
						}
						Doors.push_back(Sdoor);
						break;
					case endingdoor:
						for (int i = 0; i < Doors.size(); i++) {
							if (Doors[i].type == ending) {
								m_MDoor->RemoveModel(Doors.at(i).model);
								Doors.erase(Doors.begin()+i);
							}
						}
						DoorStruct Mdoor;
						Mdoor.model = CreateModel(m_MDoor, input,&scale,&rotation);
						Mdoor.state = DOOR_CLOSED;
						Mdoor.type = ending;
						Mdoor.movementSpeed = 0.25;
						Mdoor.areaLength.y = 65 * scale;
						Mdoor.length.y = 85 * scale;
						Mdoor.MaxDoorLimit = 16.5;
						Mdoor.CurrentDoorLimit = 0;
						if (rotation == 0 || rotation == 180) {
							Mdoor.areaLength.x = 33 * scale;
							Mdoor.areaLength.z = 65 * scale;
							Mdoor.length.x = 13 * scale;
							Mdoor.length.z = 33 * scale;
						}
						else if (rotation == 90 || rotation == 270) {
							Mdoor.areaLength.x = 65 * scale;
							Mdoor.areaLength.z = 33 * scale;
							Mdoor.length.x = 33 * scale;
							Mdoor.length.z = 13 * scale;
						}
						Doors.push_back(Mdoor);
						break;
					case startingdoor:
						for (int i = 0; i < Doors.size(); i++) {
							if (Doors[i].type == starting) {
								m_MDoor->RemoveModel(Doors.at(i).model);
								Doors.erase(Doors.begin() + i);
							}
						}
						DoorStruct door;
						door.model = CreateModel(m_MDoor, input,&scale,&rotation);
						door.state = DOOR_CLOSED;
						door.type = starting;
						door.movementSpeed = 130 * scale;
						door.areaLength.y = 65 * scale;
						door.length.y =90 * scale;
						door.MaxDoorLimit = 16.5;
						door.CurrentDoorLimit = 0;
						if (rotation == 0 || rotation == 180) {
							door.areaLength.x = 33 * scale;
							door.areaLength.z = 65 * scale;
							door.length.x = 13 * scale;
							door.length.z = 33 * scale;
						}
						else if (rotation == 90 || rotation == 270) {
							door.areaLength.x = 65 * scale;
							door.areaLength.z = 33 * scale;
							door.length.x = 33 * scale;
							door.length.z = 13 * scale;
						}
						m_PlayerSPos.x = door.model->GetX();
						m_PlayerSPos.y = door.model->GetY();
						m_PlayerSPos.z = door.model->GetZ() + 3;
						Doors.push_back(door);
						break;
					case pillar:
						PillarStruct Spillar;
						Spillar.model = CreateModel(m_MPillars, input,&scale);
						Spillar.type = typePillar;
						Spillar.length.x = 0.5*scale;
						Spillar.length.y = 5*scale;
						Spillar.length.z = 0.5*scale;
						Pillars.push_back(Spillar);
						break;
					case pedestal:
						PillarStruct Spedestal;
						Spedestal.model = CreateModel(m_MPedestal, input,&scale);
						Spedestal.type = typePedestal;
						Spedestal.length.x = 2 * scale;
						Spedestal.length.y = 5 * scale;;
						Spedestal.length.z = 2 * scale;
						Pillars.push_back(Spedestal);
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
