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
	m_MCoin = myEngine->LoadMesh("Coin.x");
	m_KeyData = "";
	m_PlayerSPos = Vector(0, 0, 0);
	m_Min = Vector(INT_MAX, 0, INT_MAX);
	m_Min = Vector(INT_MIN, 0, INT_MIN);
	m_CoinCount = 0;
}

CLevel::~CLevel()
{
}

IModel* CLevel::CreateModel(IMesh* mesh, string data, bool Check, float* scale, float* rot) {
	IModel* output = mesh->CreateModel();
	float fdata[5] = { 0.0f,0.0f,0.0f,0.0f,1.0f }; // default values if no value was given for position scale or rotation
	for (int i = 0; i < 5; i++) { //check for 5 values
		int pos = data.find(",");
		fdata[i] = stof(data.substr(0, pos));
		if (data.find(",") == string::npos) //if not "," is found then we have reached the end and will get out of the for loop
			break;
		data = data.substr(pos + 1);		
	}
	//do the changes to the model according to the data provided(move it,scale it, rotate it)
	output->Scale(fdata[4]);
	output->SetPosition(fdata[0], fdata[1], fdata[2]);
	if (Check) { // if check was true then include this model to find the minimum position(bottom left) and the max position(top right)
		if (fdata[0] < m_Min.x) {
			m_Min.x = fdata[0];
		}
		if (fdata[2] < m_Min.z) {
			m_Min.z = fdata[2];
		}
		if (fdata[0] > m_Max.x) {
			m_Max.x = fdata[0];
		}
		if (fdata[2] > m_Max.z) {
			m_Max.z = fdata[2];
		}
	}
	if (rot != 0) {// if rot was provided then we want it to get the value of the rotation
		*rot = fdata[3];
	}
	if (scale != 0) {// if scale was provided then we want it to get the value of the scale
		*scale = fdata[4];
	}
	output->RotateY(fdata[3]);
	return output;
}

void CLevel::ClearLevel(vector<WallStruct>& Walls, vector<DoorStruct>& Doors,vector<PillarStruct>& Pillars,IModel*& Key, CGuard& Guard, vector<IModel*>& Coins) {
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
	while (!Coins.empty()) {
		m_MCoin->RemoveModel(Coins.back());
		Coins.pop_back();
	}
	RemoveKey(Key);
	//also clear the guards patrol points
	Guard.ClearPoints();
	//reset the variables that need to get new values
	m_KeyData = "";
	m_PlayerSPos.x = 0;
	m_PlayerSPos.y = 0;
	m_PlayerSPos.z = 0;
	m_Max.x = INT_MIN;
	m_Max.z = INT_MIN;
	m_Min.x = INT_MAX;
	m_Min.z = INT_MAX;
}

void CLevel::NextLevel(vector<WallStruct>& Walls, vector<DoorStruct>& Doors,vector<PillarStruct>& Pillars,IModel*& Key, CGuard& Guard, vector<IModel*>& Coins) {
	if (IncreaseLevelIt()) {//check if there is a next level if not then dont proceed to generating the level as it doesnt exist
		ifstream File("./Level/" + m_Levels[m_LevelIt] + ".txt");
		if (File.is_open()) { //if the file existed and we were able to open it then continue
			ClearLevel(Walls, Doors, Pillars, Key, Guard,Coins);
			enum EModelType {//enum to help decide which model the file data are refering to
				wall,
				door,
				endingdoor,
				startingdoor,
				pillar,
				pedestal,
				key,
				guard,
				coin
			};
			EModelType Current=wall;//by default the first data will be a wall unless if the file specifies otherwise
			string input;
			while (getline(File,input))
			{
				input.erase(remove_if(input.begin(), input.end(), isspace));// remove any spaces
				if (input[0] == '/'); // if it starts with / then its a comment so they code skips that line
				else if (!isalpha(input[0])) { // if it doesnt start with a letter it must be the information for the model(x,y,z,scale,rot)
					float scale;
					float rotation;
					float x;
					float z;
					int pos;
					switch (Current) {// switch for each type of model we have and different thing we have to do for each model
					case wall:
						Walls.push_back(*(new WallStruct));
						Walls.back().model = CreateModel(m_MWall, input, true, &scale, &rotation);
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
						Doors.back().model = CreateModel(m_MDoor, input, false, &scale, &rotation);
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
						for (int i = 0; i < Doors.size(); i++) {//check if there is another ending door so to delete the one that already exists
							if (Doors[i].type == ending) {
								m_MDoor->RemoveModel(Doors.at(i).model);
								Doors.erase(Doors.begin()+i);
							}
						}
						Doors.push_back(*(new DoorStruct));
						Doors.back().model = CreateModel(m_MDoor, input, false, &scale, &rotation);
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
						for (int i = 0; i < Doors.size(); i++) {//check if there is another starting door so to delete the one that already exists
							if (Doors[i].type == starting) {
								m_MDoor->RemoveModel(Doors.at(i).model);
								Doors.erase(Doors.begin() + i);
							}
						}
						Doors.push_back(*(new DoorStruct));
						Doors.back().model = CreateModel(m_MDoor, input, false, &scale, &rotation);
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
					case pillar:
						Pillars.push_back(*(new PillarStruct));
						Pillars.back().model = CreateModel(m_MPillars, input, true, &scale);
						Pillars.back().type = typePillar;
						Pillars.back().length.x = 0.5*scale;
						Pillars.back().length.y = 5*scale;
						Pillars.back().length.z = 0.5*scale;
						break;
					case pedestal:
						Pillars.push_back(*(new PillarStruct));
						Pillars.back().model = CreateModel(m_MPedestal, input, true, &scale);
						Pillars.back().type = typePedestal;
						Pillars.back().length.x = 2 * scale;
						Pillars.back().length.y = 5 * scale;;
						Pillars.back().length.z = 2 * scale;
						break;
					case key:
						if (Key != NULL)//if there is a key already delete it and then create the new key
							m_MKey->RemoveModel(Key);
						Key = CreateModel(m_MKey, input, false);
						Key->RotateX(90);
						m_KeyData = input;
						break;
					case guard://creates points for the guard to go for his patrol
						pos = input.find(",");
						x = stof(input.substr(0, pos));
						input = input.substr(pos + 1);
						pos = input.find(",");
						z = stof(input.substr(0, pos));
						Guard.CreatePoint(Vector(x, 0, z));
						break;
					case coin:
						Coins.push_back(CreateModel(m_MCoin, input, false));
						Coins.back()->RotateX(90);
						m_CoinCount++;
						break;
					}
				}
				else {//if it starts with a character then it means that we have a keyword to change the models we create
					for (int i = 0; i < input.size(); i++) {//convert all letters to lower case
						input[i] = tolower(input[i]);
					}
					//change the current to the one that the input corresponds to
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
					else if (input == "guard")
						Current = guard;
					else if (input == "key")
						Current = key;
					else if (input == "coin")
						Current = coin;
				}
			}
			//Create the grid and then set the position of the guard
			CreateGrid(Walls, Pillars, Guard);
			Guard.SetPosition();
		}
		else {
			cout << "File not found" << endl;
		}
	}
}

void CLevel::ReloadKey(IModel*& Key) {
	if (Key == NULL) {//if the key was deleted then recreate it
		Key = CreateModel(m_MKey, m_KeyData, false);
		Key->RotateX(90);
	}
}

void CLevel::CreateGrid(vector<WallStruct> Walls, vector<PillarStruct> Pillars, CGuard& Guard) {
	vector<vector<int>> Grid;
	int heightofgrid = (m_Max.z - m_Min.z) / 5;
	int widthofgrid = (m_Max.x - m_Min.x) / 5;
	//create the grid to the size specifid above and fill it with 1(meaning no obstacle)
	vector<int> line;
	line.resize(widthofgrid);
	fill(line.begin(), line.end(), 1);
	Grid.resize(heightofgrid);
	fill(Grid.begin(), Grid.end(), line);
	for (int i = 0; i < Walls.size(); i++) {// where there is a wall in the grid set it to 0
		int maxX = (Walls[i].model->GetX() + Walls[i].length.x - m_Min.x) / 5;
		int maxZ = (Walls[i].model->GetZ() + Walls[i].length.z - m_Min.z) / 5;
		int minX = (Walls[i].model->GetX() - Walls[i].length.x - m_Min.x) / 5;
		int minZ = (Walls[i].model->GetZ() - Walls[i].length.z - m_Min.z) / 5;
		for (int k = minX; k <= maxX; k++) {
			for (int l = minZ; l <= maxZ; l++) {
				if (l < Grid.size() && l>=0)
					if (k < Grid[l].size() && k>=0)
						Grid[l][k] = 0;
			}
		}
	}
	for (int i = 0; i < Pillars.size(); i++) {// where tehre is a pillar in the grid set it to 0
		int maxX = (Pillars[i].model->GetX() + Pillars[i].length.x - m_Min.x) / 5;
		int maxZ = (Pillars[i].model->GetZ() + Pillars[i].length.z - m_Min.z) / 5;
		int minX = (Pillars[i].model->GetX() - Pillars[i].length.x - m_Min.x) / 5;
		int minZ = (Pillars[i].model->GetZ() - Pillars[i].length.z - m_Min.z) / 5;
		for (int k = minX; k <= maxX; k++) {
			for (int l = minZ; l <= maxZ; l++) {
				if (l < Grid.size() && l>=0)
					if (k < Grid[l].size() && k>=0)
						Grid[l][k] = 0;
			}
		}
	}
	//sent the grid to the guard class
	Guard.SetGrid(Grid);
}

bool CLevel::IncreaseLevelIt() {
	m_LevelIt++;
	if (m_LevelIt < m_Levels.size())//if the iterator is in the range of the levels then return true else return false
		return true;
	else
		return false;
}

void CLevel::RemoveKey(IModel*& Key) {
	if (Key != NULL) {//if the key exists delete it
		m_MKey->RemoveModel(Key);
		Key = NULL;
	}
}

void CLevel::RemoveCoin(IModel*& Coin) {
	if(Coin != NULL) {//if the coin exists delete it
		m_MCoin->RemoveModel(Coin);
	}
}

void CLevel::Restart(vector<WallStruct>& Walls, vector<DoorStruct>& Doors, vector<PillarStruct>& Pillars, IModel*& Key, CGuard& Guard, vector<IModel*>& Coins) {
	m_LevelIt = -1;
	m_CoinCount = 0;
	NextLevel(Walls, Doors, Pillars, Key, Guard,Coins);
}
