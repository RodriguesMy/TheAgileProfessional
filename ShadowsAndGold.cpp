// ShadowsAndGold.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
using namespace tle;
#include <iostream>
#include <time.h>

#include "CThief.h"
void main()
{
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine( kTLX );
	myEngine->StartWindowed();

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder( "./Media" );

	/**** Set up your scene here ****/
	ICamera* camera = myEngine->CreateCamera(kManual, 0, 15, -25);
	camera->RotateX(25);

	//IMPORTANT VARIABLES
	float dt;
	float thiefMovementSpeed = 10;

	//Create Thief
	CThief* Thief = new CThief(myEngine);

	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// Draw the scene
		myEngine->DrawScene();

		/**** Update your scene each frame here ****/
		//thiefControls
		dt = myEngine->Timer();
		/*if (myEngine->KeyHeld(Key_W)) {
			thief->MoveLocalZ(thiefMovementSpeed*dt);
		}
		if (myEngine->KeyHeld(Key_S)) {
			thief->MoveLocalZ(-thiefMovementSpeed * dt);
		}
		if (myEngine->KeyHeld(Key_D)) {
			thief->MoveLocalX(thiefMovementSpeed*dt);
		}
		if (myEngine->KeyHeld(Key_A)) {
			thief->MoveLocalX(-thiefMovementSpeed * dt);
		}*/
	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}
