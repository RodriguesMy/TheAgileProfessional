// ShadowsAndGold.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <iostream>
#include"Level.h"
using namespace tle;


void UpdateModel(I3DEngine* myEngine,IModel* pThief,float thiefMovementSpeed,float &dt)
{
	if (myEngine->KeyHeld(Key_W)) {
		pThief->MoveLocalZ(thiefMovementSpeed*dt);
	}
	if (myEngine->KeyHeld(Key_S)) {
		pThief->MoveLocalZ(-thiefMovementSpeed * dt);
	}
	if (myEngine->KeyHeld(Key_D)) {
		pThief->MoveLocalX(thiefMovementSpeed*dt);
	}
	if (myEngine->KeyHeld(Key_A)) {
		pThief->MoveLocalX(-thiefMovementSpeed * dt);
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

	IMesh* pDummyMesh = myEngine->LoadMesh("dummy.x");
	IModel* pCameraDummy = pDummyMesh->CreateModel();

	ICamera* camera = myEngine->CreateCamera();
	camera->RotateX(25);

	CLevel levels(myEngine);

	//Model Containers
	vector<IModel*> walls;
	vector<IModel*> doors;
	IModel* maindoor = 0;

	levels.NextLevel(walls, doors, maindoor);

	//NON-IMPORTANT VARIABLES
	float dt;
	float thiefMovementSpeed = 10;

	//Create Thief
	IMesh* pThieflMesh = myEngine->LoadMesh("sierra.x");
	IModel* pThief = pThieflMesh->CreateModel();
	
	//Rotation of camera variables
	float maxCameraRotation = 40;
	float cameraAngle = 25;
	float minCameraRotation = 0;

	//END OF NON-IMPORTANT VARIABLES 

	//Setting Position & Attaching models
	camera->SetPosition(pThief->GetX(), pThief->GetY() + 3, pThief->GetZ() - 3);

	camera->AttachToParent(pCameraDummy);
	pCameraDummy->AttachToParent(pThief);

	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// Draw the scene
		myEngine->DrawScene();
		dt = myEngine->Timer();

		/**** Update your scene each frame here ****/
		UpdateModel(myEngine, pThief, thiefMovementSpeed, dt);
		UpdateCamera(myEngine, pThief, cameraAngle, maxCameraRotation, pCameraDummy, minCameraRotation);
		
	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}
