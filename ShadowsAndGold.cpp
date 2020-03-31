// ShadowsAndGold.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <iostream>
#include"Level.h"
using namespace tle;

#define MENU 1
#define LEVEL 2
#define FINAL_CUTSCENE 3
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
	CLevel &levels, vector<IModel*> walls, vector<IModel*> doors,vector<IModel*> pillars, IModel* maindoor, IModel* Key, int &currentLevel) {
	
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
		levels.NextLevel(walls, doors, pillars, maindoor,Key);
		mainDoorUnlocked = false;
	}
}
//Collision detection with walls
bool SphereToBoxCD(IModel* pThief, vector<IModel*> walls, float wallsXLength, float wallsZLength) {
	for (int i = 0; i < walls.size(); i++) {
		
		if (pThief->GetX() < walls[i]->GetX() + wallsXLength && pThief->GetX() > walls[i]->GetX() - wallsXLength &&
			/*pThief->GetY() < walls[i]->GetY() + wallsYLength && pThief->GetY() > walls[i]->GetY() - wallsYLength &&*/
			pThief->GetZ() < walls[i]->GetZ() + wallsZLength && pThief->GetZ() > walls[i]->GetZ() - wallsZLength){
			cout << "wall" << endl;
			return true;
		}
	}
	return false;
}
//Collision detection for key
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
	IModel* pCameraDummy = pDummyMesh->CreateModel(0, 0, -5);

	CLevel levels(myEngine);

	//Model Containers
	vector<IModel*> walls;
	vector<IModel*> doors;
	vector<IModel*> pillars;

	IModel* maindoor=0;
	IModel* key=0;

	int STATE = MENU;
	levels.NextLevel(walls, doors,pillars, maindoor,key);

	//NON-IMPORTANT VARIABLES
	float dt=myEngine->Timer();
	float const thiefMovementSpeed = 5;

	//Create Thief
	IMesh* pThieflMesh = myEngine->LoadMesh("thief.x");
	IModel* pThief = pThieflMesh->CreateModel(0, 0, -5);
	pThief->Scale(5);
	ICamera* camera = myEngine->CreateCamera(kManual);
	camera->RotateX(25);

	//IFONT Variables
	IFont* DisplayGameName = myEngine->LoadFont("Cambria", 120U);
	IFont* DisplayMenu = myEngine->LoadFont("Cambria", 70U);
	//Message Displaying variables
	IFont* DisplayQuest = myEngine->LoadFont("Cambria", 24U);
	IFont* InteractionMessage = myEngine->LoadFont("Cambria", 24U);
	//END OF IFONT Variables

	//Rotation of camera variables
	float const maxCameraRotation = 30;
	float cameraAngle = 25;
	float const minCameraRotation = 20;

	//Key related variables
	bool keyFound = false;

	//Door variables
	bool mainDoorUnlocked = false;
	bool simpleDoorNearby = false;
	bool mainDoorNearby = false;

	//Wall variables
	float const wallXLength = 0.3;
	/*float const wallYLength = 10;*/
	float const wallZLength = 5;

	//END OF NON-IMPORTANT VARIABLES 
	camera->SetPosition(pThief->GetX(), pThief->GetY()+2.5, pThief->GetZ()-2);
	camera->AttachToParent(pCameraDummy);
	pCameraDummy->AttachToParent(pThief);
	pCameraDummy->RotateY(180);

	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// Draw the scene
		myEngine->DrawScene();
		dt = myEngine->Timer();
		//i need the number of walls 

		/**** Update your scene each frame here ****/
		myEngine->StartMouseCapture(); // Disables mouse and centers it in the center of the screen 
		InteractionMessage->Draw("Press 'E' to open.", 565, 550);

		switch (STATE)
		{
		case MENU:
		{
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
			//Myriam, testing
			if (myEngine->KeyHeld(Key_W) && !SphereToBoxCD(pThief, walls, wallXLength,wallZLength)) {
				pThief->MoveLocalZ(-thiefMovementSpeed * dt);
			}
			//end of Myriam testing
			UpdateModel(myEngine, pThief, thiefMovementSpeed, dt);
			UpdateCamera(myEngine, pThief, cameraAngle, maxCameraRotation, pCameraDummy, minCameraRotation);
			if (myEngine->KeyHit(Key_P))
				if (levels.NextLevel(walls, doors, pillars, maindoor, key))
					cout << "no more levels" << endl;
			UpdateLevel(keyFound, DisplayQuest, simpleDoorNearby, InteractionMessage, myEngine, mainDoorNearby, mainDoorUnlocked, levels, walls, doors, pillars, maindoor, key, STATE);
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