// ShadowsAndGold.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <iostream>
#include"Level.h"
using namespace tle;

#define MENU 1
#define LEVEL 2
#define PLAYER_LOST 3
#define LOADING_NEXT_LEVEL 4

bool BooleanBoxCD(IModel* model1,IModel* model2,float modelXLength,float modelYLength,float modelZLength)
{
	return ((model1->GetX() < model2->GetX() + modelXLength && model1->GetX() > model2->GetX() - modelXLength &&
		model1->GetY() < model2->GetY() + modelYLength && model1->GetY() > model2->GetY() - modelYLength &&
		model1->GetZ() < model2->GetZ() + modelZLength && model1->GetZ() > model2->GetZ() - modelZLength));
}
bool CollisionWithWalls(IModel* pThief, vector<WallStruct> walls) {
	for (int i = 0; i < walls.size(); i++) {
		
		if (BooleanBoxCD(pThief,walls[i].model,walls[i].wallXLength,walls[i].wallYLength,walls[i].wallZLength)){
			return true;
		}
	}
	return false;
}
bool CollisionWithDoors(IModel* pThief, vector<DoorStruct> doors) {

	for (int i = 0; i < doors.size(); i++) {

		if (BooleanBoxCD(pThief, doors[i].model, doors[i].doorXLength, doors[i].doorYLength, doors[i].doorZLength)) {
			return true;
		}
	}
	return false; 
}
bool CollisionWithPillars(IModel* pThief, vector<PillarStruct> pillars) {

	for (int i = 0; i < pillars.size(); i++) {

		if (BooleanBoxCD(pThief, pillars[i].model, pillars[i].pillarXLength, pillars[i].pillarYLength, pillars[i].pillarZLength)) {
			return true;
		}
	}
	return false;
}
void UpdateModel(I3DEngine* myEngine,IModel* pThief,float &thiefMovementSpeed,float &dt,vector<WallStruct> walls,vector<PillarStruct> pillars)
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

	//Movement depends on collision with:
	//WALLS, DOORS, PILLARS
	if (CollisionWithWalls(pThief, walls) || CollisionWithPillars(pThief,pillars))/*|| CollisionWithDoor(pThief,doors)*/ {
		if (myEngine->KeyHeld(Key_W)) {
			pThief->MoveLocalZ(thiefMovementSpeed * dt);
		}
		if (myEngine->KeyHeld(Key_S)) {
			pThief->MoveLocalZ(-thiefMovementSpeed * dt);
		}
		if (myEngine->KeyHeld(Key_D)) {
			pThief->MoveLocalX(thiefMovementSpeed * dt);
		}
		if (myEngine->KeyHeld(Key_A)) {
			pThief->MoveLocalX(-thiefMovementSpeed * dt);
		}
	}
	if (myEngine->KeyHeld(Key_Shift))
	{
		thiefMovementSpeed = 7;
	}
	else
	{
		thiefMovementSpeed = 5;
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
void UpdateMessages(bool &keyFound,IFont* DisplayQuest,IFont* InteractionMessage, IFont* ControlsMessage,float &currentTime,float maxTimer,float dt) {
	
	//Update the Quests Fonts
	if (keyFound)
	{
		DisplayQuest->Draw("You Found the Key! Find the Door to Go to the Next Floor.", 0, 0);
	}
	else
	{
		DisplayQuest->Draw("Quest: Avoid Guards and Find the Key to Unlock Next Floor.", 0, 0);
	}
	currentTime += dt;
	if (currentTime < maxTimer)
	{
		ControlsMessage->Draw("Use WASD to move", 0, 550);
		ControlsMessage->Draw("Hold Left Shift\nto run", 0, 600);
	}
}

void UpdateDoor(EDoorState& doorState, IModel* door, int maxLimit, float& currentLimit, IFont* InteractionMessage, I3DEngine* myEngine, float doorMovementSpeed, float dt,
	bool keyFound, EDoortype doorType,IModel* pThief,float doorXLength,float doorYLength, float doorZLength)
{
	/*MAIN SWITCH STATEMENT FOR DOORS
	-Each door has its own state
	DOOR_CLOSED:
	1. Checks for collision detection with the current door sent by a parameter
	2. If there is collision, it checks the door type so it can act accordingly.
		Simple Door: You can simply open the door.
		Starting Door: You can not open the starting door. 
		Ending Door: You can open the ending door only if you have the key,
					otherwise, a message is displayed saying that the player has to find the key.

	DOOR_OPEN:
	1. Checks for collision detection with the current door sent by a parameter
	2. If there is a collision, it checks the door type so it can act accordingly.
		Simple Door: You can simply close the door
		Starting Door: This door can not be opened, therefore it can not be closed    //MIGHT CHANGE LATER
		Ending Door:                                                                  //ADD LATER

	DOOR_OPENING:
	1. Simply moves the door's local Z to a maximum limit

	DOOR_CLOSING:
	1. Simply moves the door's local Z to a maximum limit
	*/
	switch (doorState)
	{
	case DOOR_CLOSED:
	{
		if (BooleanBoxCD(pThief,door,doorXLength,doorYLength,doorZLength)) {

			if (doorType == simple) {
				InteractionMessage->Draw("Press 'E' to open.", 565, 550);
				if (myEngine->KeyHit(Key_E))
				{
					doorState = DOOR_OPENING;
				}
			}

			if (doorType == ending && keyFound) {
				InteractionMessage->Draw("Press 'E' to go to the next level.", 565, 550);
				if (myEngine->KeyHit(Key_E))
				{
					doorState = DOOR_OPENING;
					//progress to the next level
				}
			}

			if (doorType == ending && !keyFound) {
				InteractionMessage->Draw("You have to find the key first.", 565, 550);
			}
			if (doorType == starting && !keyFound) {
				InteractionMessage->Draw("No turning back now.", 565, 550);
			}
		}
		
	}break;
	case DOOR_OPEN:
	{
		if (BooleanBoxCD(pThief, door, doorXLength, doorYLength, doorZLength)) {
			if (doorType == simple) {
				InteractionMessage->Draw("Press 'E' to close.", 565, 550);
				if (myEngine->KeyHit(Key_E))
				{
					doorState = DOOR_CLOSING;
				}
			}
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
void CollisionToHandleDoors(IModel* pThief, vector<DoorStruct>& door, IFont* InteractionMessage,
	I3DEngine* myEngine,float dt,bool keyFound, float &CurrentDoorLimit, float MaxDoorLimit) {
	for (int i = 0; i < door.size(); i++) {
		UpdateDoor(door[i].state, door[i].model, MaxDoorLimit, CurrentDoorLimit, InteractionMessage, myEngine, door[i].doorMovementSpeed, dt,
			keyFound, door[i].type,pThief, door[i].doorXLengthArea, door[i].doorYLengthArea, door[i].doorZLengthArea);
	}
}
void CollisionWithKey(IModel* pThief, float R1, float R2,CLevel level,bool &keyFound) {

	if (!keyFound) {
		float x = pThief->GetX() -level.getKey()->GetX();
		float y = pThief->GetY() - level.getKey()->GetY();
		float z = pThief->GetZ() - level.getKey()->GetZ();

		if (sqrt(x * x + y * y + z * z) < R1 + R2) {
			keyFound = true;
			level.RemoveKey();
		}
	}
		
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
	vector<PillarStruct> pillars;

	IModel* key=0;

	int STATE = MENU;
	levels.NextLevel(walls, doors,pillars,key);

	//NON-IMPORTANT VARIABLES
	float dt=myEngine->Timer();
	float thiefMovementSpeed = 5;

	//Create Thief
	IMesh* pThieflMesh = myEngine->LoadMesh("thief.x");
	IModel* pThief = pThieflMesh->CreateModel(0, 0, -10);
	pThief->Scale(5);
	ICamera* camera = myEngine->CreateCamera(kManual,0,2,-3);
	camera->RotateX(25);

	//IFONT Variables
	IFont* DisplayBigMessage = myEngine->LoadFont("Cambria", 120U);
	IFont* DisplayMenu = myEngine->LoadFont("Cambria", 70U);
	//Message Displaying variables
	IFont* DisplayQuest = myEngine->LoadFont("Cambria", 24U);
	IFont* InteractionMessage = myEngine->LoadFont("Cambria", 24U);
	IFont* ControlsMessage = myEngine->LoadFont("Cambria", 24U);
	float currentTime = 0;
	float const maxTimer = 10;
	//END OF IFONT Variables

	//Rotation of camera variables
	float const maxCameraRotation = 35;
	float cameraAngle = 25;
	float const minCameraRotation = 10;

	//Key related variables
	bool keyFound = false;

	//Door variables
	float CurrentDoorLimit=0;
	float MaxDoorLimit=25;

	//END OF NON-IMPORTANT VARIABLES 
	camera->AttachToParent(pCameraDummy);
	pCameraDummy->AttachToParent(pThief);
	pCameraDummy->RotateY(180);

	//Key variables
	levels.SetUpKey();
	float R1 = 5;
	float R2 = 7;
	float keyMovementSpeed = 150;

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
			DisplayBigMessage->Draw("Shadows & Gold", 300, 300);
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
			myEngine->StartMouseCapture(); // Disables mouse moving and centers it in the center of the screen 
			CollisionToHandleDoors(pThief,doors,InteractionMessage,myEngine,dt,keyFound, CurrentDoorLimit, MaxDoorLimit);
			CollisionWithKey(pThief, R1, R2, levels, keyFound);
			levels.UpdateKey(keyMovementSpeed,dt,keyFound);			
			UpdateModel(myEngine, pThief, thiefMovementSpeed, dt,walls, pillars);
			UpdateCamera(myEngine, pThief, cameraAngle, maxCameraRotation, pCameraDummy, minCameraRotation);
			if (myEngine->KeyHit(Key_P))
				if (levels.NextLevel(walls, doors, pillars, key))
					cout << "no more levels" << endl;
			UpdateMessages(keyFound, DisplayQuest,InteractionMessage,ControlsMessage,currentTime,maxTimer,dt);		
			break;
		}
		case PLAYER_LOST:
		{
			DisplayBigMessage->Draw("You Lost!", 300, 300);
			DisplayMenu->Draw("Hit Space to Try Again!", 420, 450);
			if (myEngine->KeyHit(Key_Space))
			{
				STATE = LEVEL;
				//reset variables
			}
			break;
		}
		case LOADING_NEXT_LEVEL:
		{
			// This state could execute when we want to render the next level smoothly
			DisplayBigMessage->Draw("Loading . . .", 300, 300);
			//I thought of maybe putting a wall in front of the camera (as a black screen) and changing the level here
			//Resetting the player's and guard's position
			//Having the starting place of the player and the guard inside each level (get them from level.txt) is a good idea
			//for having checkpoints

			if (1/*loading map finished*/)
			{
				STATE = LEVEL;
			}
		}
		break;
		}


		if (myEngine->KeyHit(Key_Escape)) {
			myEngine->Stop();
		}
	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}