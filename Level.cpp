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

IModel* CLevel::CreateModel(IMesh* mesh,string data,float* rot) {
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
				if (input[0] == '/');
				else if (!isalpha(input[0])) {
					switch (Current) {
					case wall:
						WallStruct wall;
						wall.model = CreateModel(m_MWall, input, &(wall.rot));
						if (wall.rot == 0 || wall.rot == 180)
						{
							wall.wallXLength = 11;
							wall.wallYLength = 10;
							wall.wallZLength = 2;
						}
						if (wall.rot == 90 || wall.rot == 270)
						{
							wall.wallXLength = 2;
							wall.wallYLength = 15;
							wall.wallZLength = 11;
						}
						Walls.push_back(wall);
						break;
					case door:
						DoorStruct Sdoor;
						Sdoor.model = CreateModel(m_MDoor, input,&(Sdoor.rot));
						Sdoor.state = DOOR_CLOSED;
						Sdoor.type = simple;
						Sdoor.doorXLengthArea = 5;
						Sdoor.doorYLengthArea = 10;
						Sdoor.doorZLengthArea = 10;
						Sdoor.doorMovementSpeed = 20;
						if (Sdoor.rot == 0 || Sdoor.rot == 180)
						{
							Sdoor.doorXLength=2;
							Sdoor.doorYLength=10;
							Sdoor.doorZLength=5;
						}
						if (Sdoor.rot == 90 ||  Sdoor.rot==270)
						{
							Sdoor.doorXLength = 5;
							Sdoor.doorYLength = 10;
							Sdoor.doorZLength = 2;
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
						Mdoor.model = CreateModel(m_MDoor, input,&(Mdoor.rot));
						Mdoor.state = DOOR_CLOSED;
						Mdoor.type = ending;
						Mdoor.doorXLengthArea = 5;
						Mdoor.doorYLengthArea = 10;
						Mdoor.doorZLengthArea = 10;
						Mdoor.doorMovementSpeed = 20;
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
						door.model = CreateModel(m_MDoor, input,&(door.rot));
						door.state = DOOR_CLOSED;
						door.type = starting;
						door.doorXLengthArea = 5;
						door.doorYLengthArea = 10;
						door.doorZLengthArea = 10;
						door.doorMovementSpeed = 20;
						m_PlayerSPos.x = door.model->GetX();
						m_PlayerSPos.y = door.model->GetY();
						m_PlayerSPos.z = door.model->GetZ() + 3;
						Doors.push_back(door);
						break;
					case pillar:
						PillarStruct Spillar;
						Spillar.model = CreateModel(m_MPillars, input);
						Spillar.type = typePillar;
						Spillar.pillarXLength = 1;
						Spillar.pillarYLength = 10;
						Spillar.pillarZLength = 1;
						Pillars.push_back(Spillar);
						break;
					case pedestal:
						PillarStruct Spedestal;
						Spedestal.model = CreateModel(m_MPedestal, input);
						Spedestal.type = typePedestal;
						Spedestal.pillarXLength = 3;
						Spedestal.pillarYLength = 10;
						Spedestal.pillarZLength = 3;
						Pillars.push_back(Spedestal);
						break;
					case key:
						if (Key != NULL)
							m_MKey->RemoveModel(Key);
						m_Key = CreateModel(m_MKey, input);
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

void CLevel::RemoveKey() {
	m_MKey->RemoveModel(m_Key);
}

IModel* CLevel::getKey() {
	return m_Key;
}

void CLevel::UpdateKey(float keyMovingSpeed,float dt,bool keyFound) {
	if(!keyFound)
	m_Key->RotateY(keyMovingSpeed*dt);
}

void CLevel::SetUpKey() {
	m_Key->RotateX(90);
	m_Key->SetY(7);
}
