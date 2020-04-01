// ShadowsAndGold.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <iostream>
#include"Level.h"
using namespace tle;

#define MENU 1
#define LEVEL 2
#define FINAL_CUTSCENE 3

//Door variables
#define DOOR_OPENING 0
#define DOOR_CLOSING 1
#define DOOR_CLOSED 3
#define DOOR_OPEN 4

void UpdateModel(I3DEngine* myEngine,IModel* pThief,float thiefMovementSpeed,float &dt)
{

	if (myEngine->KeyHeld(Key_W)) {
		pThief->MoveLocalZ(-thiefMovementSpeed * dt);
	}
	if (myEngine->KeyHeld(Key_S)) {
		pThief->MoveLocalZ(thiefMovementSpeed * dt);
	}
	if (myEngine->KeyHeld(Key_D)) {
		pThief->MoveLocalX(-thiefMovementSpeed * dt);
	}
	if (myEngine->KeyHeld(Key_A)) {
		pThief->MoveLocalX(thiefMovementSpeed * dt);
	}
	
}
void UpdateCamera(I3DEngine* myEngine,IModel* pThief,float &cameraAngle,float maxCameraRotation,IModel* pCameraDummy,float minCameraRotation)
{
	float cameraMovementY = myEngine->GetMouseMovementY();
	float cameraMovementX = myEngine->GetMouseMovementX();

	pThief->RotateY(cameraMovementX);
	//pCameraDummy->RotateY(cameraMovementX);

	if (cameraMovementY > 0)
	{
		if (cameraAngle < maxCameraRotation)
		{
			pCameraDummy->RotateLocalX(cameraMovementY);
			cameraAngle += cameraMovementY;
		}
	}
	else if (cameraMovementY < 0) {
		if (cameraAngle > minCameraRotation) {
			pCameraDummy->RotateLocalX(cameraMovementY);
			cameraAngle += cameraMovementY;
		}
	}
}
void UpdateLevel(bool &keyFound,IFont* DisplayQuest,bool &simpleDoorNearby,IFont* InteractionMessage,I3DEngine* myEngine, bool &mainDoorNearby,bool &mainDoorUnlocked,
	CLevel &levels, vector<WallStruct> walls, vector<DoorStruct>& doors,vector<IModel*> pillars, IModel* Key, int &currentLevel) {
	
	//Update the Quests Fonts
	if (keyFound)
	{
		DisplayQuest->Draw("You Found the Key! Find the Door to Go to the Next Floor.", 0, 0);
	}
	else
	{
		DisplayQuest->Draw("Quest: Avoid Guards and Find the Key to Unlock Next Floor.", 0, 0);
	}

	//If door is nearby there should be a message displaying the interaction 
	if (simpleDoorNearby/*implement collision detection to change this value*/)
	{
		InteractionMessage->Draw("Press 'E' to open.", 565, 550);

		if (myEngine->KeyHit(Key_E))
		{
			//open door (implement doors opening/sliding)
		}
	}

	if (mainDoorNearby/*implement collision detection to change this value*/)
	{
		InteractionMessage->Draw("Press 'E' to Unlock Next Floor.", 530, 550);
		if (myEngine->KeyHit(Key_E))
		{
			//open door (implement doors opening/sliding)
			mainDoorUnlocked = true; //this will cause the transition 
		}
	}

	//Transition
	if (mainDoorUnlocked)
	{
		keyFound = false;
		mainDoorUnlocked = false;
		levels.NextLevel(walls, doors, pillars,Key);
		mainDoorUnlocked = false;
	}
}
bool CollisionWithWalls(IModel* pThief, vector<WallStruct> walls, float wallsXLength, float wallsYLength, float wallsZLength) {
	for (int i = 0; i < walls.size(); i++) {
		
		if (pThief->GetX() < walls[i].model->GetX() + wallsXLength && pThief->GetX() > walls[i].model->GetX() - wallsXLength &&
			pThief->GetY() < walls[i].model->GetY() + wallsYLength && pThief->GetY() > walls[i].model->GetY() - wallsYLength &&
			pThief->GetZ() < walls[i].model->GetZ() + wallsZLength && pThief->GetZ() > walls[i].model->GetZ() - wallsZLength){
			cout << "wall" << endl;
			return true;
		}
	}
	return false;
}
void UpdateDoor(int& doorState, IModel* door, int maxLimit, float& currentLimit, IFont* InteractionMessage, I3DEngine* myEngine, float doorMovementSpeed, float dt,bool startingDoor,bool endingDoor)
{
	switch (doorState)
	{
	case DOOR_CLOSED:
	{
		InteractionMessage->Draw("Press 'E' to open.", 565, 550);
		if (myEngine->KeyHit(Key_E))
		{
			doorState = DOOR_OPENING;
		}
	}break;
	case DOOR_OPEN:
	{
		InteractionMessage->Draw("Press 'E' to close.", 565, 550);
		if (myEngine->KeyHit(Key_E))
		{
			doorState = DOOR_CLOSING;
		}
	}break;
	case DOOR_CLOSING:
	{
		door->MoveLocalZ(-doorMovementSpeed * dt);
		currentLimit += 0.1;
		if (currentLimit > maxLimit)
		{
			doorState = DOOR_CLOSED;
			currentLimit = 0;
		}

	}break;
	case DOOR_OPENING:
	{
		door->MoveLocalZ(doorMovementSpeed * dt);
		currentLimit += 0.1;
		if (currentLimit > maxLimit)
		{
			doorState = DOOR_OPEN;
			currentLimit = 0;
		}
	}break;
	}
}
void CollisionWithDoors(IModel* pThief, vector<DoorStruct>& door, float doorXLength, float doorYLength, float doorZLength,int maxLimit,float &currentLimit, IFont* InteractionMessage,
	I3DEngine* myEngine,float doorMovementSpeed,float dt) {

	for (int i = 0; i < door.size(); i++) {

		if (pThief->GetX() < door[i].model->GetX() + doorXLength && pThief->GetX() > door[i].model->GetX() - doorXLength &&
			pThief->GetY() < door[i].model->GetY() + doorYLength && pThief->GetY() > door[i].model->GetY() - doorYLength &&
			pThief->GetZ() < door[i].model->GetZ() + doorZLength && pThief->GetZ() > door[i].model->GetZ() - doorZLength) {
			if (door[i].type == simple) //normal door opening
				UpdateDoor(door[i].state, door[i].model, maxLimit, currentLimit, InteractionMessage, myEngine, doorMovementSpeed, dt,false,false);
			
			if (door[i].type == starting) //not being able to open it
				UpdateDoor(door[i].state, door[i].model, maxLimit, currentLimit, InteractionMessage, myEngine, doorMovementSpeed, dt, true, false);
			
			if (door[i].type == ending) //opening this door loads the next level and sets the ending door as a starting door
				UpdateDoor(door[i].state, door[i].model, maxLimit, currentLimit, InteractionMessage, myEngine, doorMovementSpeed, dt, false, true);
			
		}
			
		}
	}
bool SphereToSphereCD(IModel* pThief, IModel* key, float R1, float R2) {

	float x = pThief->GetX() - key->GetX();
	float y = pThief->GetY() - key->GetY();
	float z = pThief->GetZ() - key->GetZ();

	if (sqrt(x * x + y * y + z * z) < R1 + R2) {
		return true;
	}

	return false;
}
void main()
{
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine( kTLX );
	myEngine->StartWindowed();
	// Add default folder for meshes and other media
	myEngine->AddMediaFolder( "./Media" );

	/**** Set up your scene here ****/
	IMesh* pFloorMesh = myEngine->LoadMesh("Floor.x");
	IModel* pFloor = pFloorMesh->CreateModel(0,-0.3,0);
	
	IMesh* pRoofMesh = myEngine->LoadMesh("Floor.x");
	IModel* pRoof = pFloorMesh->CreateModel(0, 16, 0);
	pRoof->RotateZ(180);

	IMesh* pDummyMesh = myEngine->LoadMesh("dummy.x");
	IModel* pCameraDummy = pDummyMesh->CreateModel(0,0.8, 0);

	CLevel levels(myEngine);

	//Model Containers
	vector<WallStruct> walls;
	vector<DoorStruct> doors;
	vector<IModel*> pillars;

	IModel* key=0;

	int STATE = MENU;
	levels.NextLevel(walls, doors,pillars,key);

	//NON-IMPORTANT VARIABLES
	float dt=myEngine->Timer();
	float const thiefMovementSpeed = 5;

	//Create Thief
	IMesh* pThieflMesh = myEngine->LoadMesh("thief.x");
	IModel* pThief = pThieflMesh->CreateModel(0, 0, -10);
	pThief->Scale(5);
	ICamera* camera = myEngine->CreateCamera(kManual,0,2,-3);
	camera->RotateX(25);

	//IFONT Variables
	IFont* DisplayGameName = myEngine->LoadFont("Cambria", 120U);
	IFont* DisplayMenu = myEngine->LoadFont("Cambria", 70U);
	//Message Displaying variables
	IFont* DisplayQuest = myEngine->LoadFont("Cambria", 24U);
	IFont* InteractionMessage = myEngine->LoadFont("Cambria", 24U);
	//END OF IFONT Variables

	//Rotation of camera variables
	float const maxCameraRotation = 35;
	float cameraAngle = 25;
	float const minCameraRotation = 10;

	//Key related variables
	bool keyFound = false;

	//Door variables
	bool mainDoorUnlocked = false;
	bool simpleDoorNearby = false;
	bool mainDoorNearby = false;
	float maxLimit = 30;
	float currentLimit = 0;
	float const doorXLength = 5;
	float const doorYLength = 10;
	float const doorZLength = 10;
	float const doorMovementSpeed = 20;
	vector<int> doorState;
	for (int i = 0; i < doors.size();i++)
	{
		doorState.push_back(DOOR_CLOSED);
	}
	//Wall variables
	float const wallXLength = 0.5;
	float const wallYLength = 10;
	float const wallZLength = 10;

	//END OF NON-IMPORTANT VARIABLES 
	camera->AttachToParent(pCameraDummy);
	pCameraDummy->AttachToParent(pThief);
	pCameraDummy->RotateY(180);

	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// Draw the scene
		myEngine->DrawScene();
		dt = myEngine->Timer();

		/**** Update your scene each frame here ****/
		
		switch (STATE)
		{
		case MENU:
		{	myEngine->StopMouseCapture();
			DisplayGameName->Draw("Shadows & Gold", 300, 300);
			DisplayMenu->Draw("Hit Space To Start!", 420, 450);

			InteractionMessage->Draw("Find the Keys and Get all the Gold!", 500, 600);
			if (myEngine->KeyHit(Key_Space))
			{
				STATE = LEVEL;
			}
			break;
		}
		case LEVEL:
		{
			myEngine->StartMouseCapture(); // Disables mouse and centers it in the center of the screen 

			CollisionWithDoors(pThief,doors,doorXLength,doorYLength,doorZLength,maxLimit,currentLimit,InteractionMessage,myEngine, doorMovementSpeed,dt);
			//CollisionWithWalls(pThief, walls, wallXLength, wallYLength, wallZLength);
			////Myriam, testing do not touch (trying to implement CD with walls) 
			//if (!SphereToBoxCD(pThief, walls, wallXLength, wallYLength,wallZLength)) {
			//	
			//	if(myEngine->KeyHeld(Key_W))
			//	{
			//		pThief->MoveLocalZ(-thiefMovementSpeed * dt);
			//	}				
			//}
			//if (myEngine->KeyHeld(Key_S)) {
			//	pThief->MoveLocalZ(thiefMovementSpeed * dt);
			//}
			//if (myEngine->KeyHeld(Key_D) && !SphereToBoxCD(pThief, walls, wallXLength, wallYLength, wallZLength)) {
			//	pThief->MoveLocalX(-thiefMovementSpeed * dt);
			//}
			//if (myEngine->KeyHeld(Key_A) && !SphereToBoxCD(pThief, walls, wallXLength, wallYLength, wallZLength)) {
			//	pThief->MoveLocalX(thiefMovementSpeed * dt);
			//}
			////end of Myriam testing
			UpdateModel(myEngine, pThief, thiefMovementSpeed, dt);
			UpdateCamera(myEngine, pThief, cameraAngle, maxCameraRotation, pCameraDummy, minCameraRotation);
			if (myEngine->KeyHit(Key_P))
				if (levels.NextLevel(walls, doors, pillars, key))
					cout << "no more levels" << endl;
			UpdateLevel(keyFound, DisplayQuest, simpleDoorNearby, InteractionMessage, myEngine, mainDoorNearby, mainDoorUnlocked, levels, walls, doors, pillars, key, STATE);
			
			break;
		}
		case FINAL_CUTSCENE:
		{
			//implement movement of models in the terrace
			break;
		}
		}


		if (myEngine->KeyHit(Key_Escape)) {
			myEngine->Stop();
		}
	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}