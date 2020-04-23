// ShadowsAndGold.cpp: A program using the TL-Engine

#include "Definitions.h"
#include "CollisionDetection.h"
#include "Level.h"
#include "CGuard.h"
#include <time.h>
#include <sstream>
bool CollisionWithWalls(IModel* model, vector<WallStruct> walls) {
	for (int i = 0; i < walls.size(); i++) {
		
		if (SphereToBoxCD(model,walls[i].model,walls[i].length)){
			return true;
		}
	}
	return false;
}
bool CollisionWithDoors(IModel* model, vector<DoorStruct> doors) {

	for (int i = 0; i < doors.size(); i++) {

		if (SphereToBoxCD(model, doors[i].model, doors[i].length)) {
			return true;
		}
	}
	return false; 
}
bool CollisionWithPillars(IModel* model, vector<PillarStruct> pillars) {

	for (int i = 0; i < pillars.size(); i++) {

		if (SphereToBoxCD(model, pillars[i].model, pillars[i].length)) {
			return true;
		}
	}
	return false;
}
void ThiefToGuardCD(IModel* model1, IModel* model2,int &STATE,bool &lost) {

	if (CollisionSTS(model1, model2, 3))
	{
		lost = true;
		STATE = THIEF_DIES;	
	}

}
void ThiefCollisionBehavior(I3DEngine* myEngine, vector<WallStruct> walls, vector<PillarStruct> pillars, vector<DoorStruct> doors, IModel* pThief, float& thiefMovementSpeed, float& dt)
{
	//Movement depends on collision with:
	//WALLS, DOORS, PILLARS
	if (CollisionWithWalls(pThief, walls) || CollisionWithPillars(pThief, pillars) || CollisionWithDoors(pThief, doors)) {
		/*Reverse the movement in the previous state if a collision occurs
		Basically the movement that the player was trying to do but reversed*/
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
}
void UpdateModel(I3DEngine* myEngine, IModel* pThief, float& thiefMovementSpeed, float& dt, int& ThiefState, CLevel levels, vector<DoorStruct>& doors)
{
	switch (ThiefState)
	{
	case WAITING:
		break;
	case FORWARD:
		pThief->MoveLocalZ(-thiefMovementSpeed * dt);
		if (CollisionSTS(Vector(pThief->GetX(), pThief->GetY(), pThief->GetZ()), levels.GetPlayerSPos(), 1)) {
			ThiefState = NORMAL;
			for (int i = 0; i < doors.size(); i++) {
				if (doors[i].type == starting) {
					doors[i].state = DOOR_CLOSING;
				}
			}
		}
		break;
	case NORMAL:
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

		if (myEngine->KeyHeld(Key_Shift))
		{
			thiefMovementSpeed = 7;
		}
		else
		{
			thiefMovementSpeed = 5;
		}
		break;
	}
}
void UpdateCamera(I3DEngine* myEngine, IModel* pThief, SCameraVariables& CameraV, IModel* pCameraDummy, ICamera* Camera, vector<WallStruct> walls, int ThiefState)
{
	if (ThiefState == NORMAL) {
		float cameraMovementY = myEngine->GetMouseMovementY();
		float cameraMovementX = myEngine->GetMouseMovementX();

		pThief->RotateY(cameraMovementX);

		if (cameraMovementY > 0)
		{
			if (CameraV.cameraAngle < CameraV.maxCameraRotation)
			{
				pCameraDummy->RotateLocalX(cameraMovementY);
				CameraV.cameraAngle += cameraMovementY;
			}
		}
		else if (cameraMovementY < 0) {
			if (CameraV.cameraAngle > CameraV.minCameraRotation) {
				pCameraDummy->RotateLocalX(cameraMovementY);
				CameraV.cameraAngle += cameraMovementY;
			}
		}
	}
}
void CameraCollisionBehavior(ICamera* Camera, IModel* pThief, I3DEngine* myEngine, vector<WallStruct> walls, vector<PillarStruct> pillars, vector<DoorStruct> doors, SCameraVariables& CameraV, IModel* pCameraDummy, CLevel levels)
{
	//Movement depends on collision with:
	//WALLS, DOORS
	int counter=0;
	if (CollisionWithWalls(CameraV.dummyOnCamera, walls) || 
		CollisionWithDoors(CameraV.dummyOnCamera, doors))
	{		
		if (CameraV.currentCameraDistance <= CameraV.minCameraDistance)CameraV.currentCameraDistance += 0.05;//going closer to the player	
		counter++;
	}
	
	if (CameraV.currentCameraDistance >= CameraV.maxCameraDistance && counter==0)CameraV.currentCameraDistance -= 0.05;//going away from the player
	
	Camera->SetLocalZ(CameraV.currentCameraDistance);
}
void UpdateMessages(bool& keyFound, IFont* DisplayQuest, IFont* InteractionMessage, IFont* ControlsMessage, float& currentTime, float maxTimer, float dt, CLevel levels) {
	
	//Update the Quests Fonts
	if (keyFound)
	{
		if (levels.GetLevelNumber() < 3)
		{
			DisplayQuest->Draw("You Found the Key! Find the Door to Go to the Next Floor.", 0, 0);
		}
		else {
			DisplayQuest->Draw("You Found the EXIT Key! Gather all the gold and leave!", 0, 0);
		}
	}
	else
	{
		DisplayQuest->Draw("Quest: Avoid Guards and Find the Key to Unlock Next Floor.\nGather all the gold you can!", 0, 0);
	}
	//Only display the controls for a few seconds
	currentTime += dt;
	if (currentTime < maxTimer)
	{
		ControlsMessage->Draw("Press 'P' to pause", 0, 400);
		ControlsMessage->Draw("Use your mouse\nto look around", 0, 450);
		ControlsMessage->Draw("Use WASD to move", 0, 550);
		ControlsMessage->Draw("Hold Left Shift\nto run", 0, 600);		
	}
	
}
void UpdateDoor(EDoorState& doorState, IModel* door, int maxLimit, float& currentLimit, IFont* InteractionMessage, I3DEngine* myEngine, float doorMovementSpeed, bool& keyFound, EDoortype doorType, IModel* pThief, Vector areaLength, CLevel& levels, vector<WallStruct>& walls, vector<DoorStruct>& doors, vector<PillarStruct>& pillars, IModel*& key, int& ThiefState, int& STATE, bool& finished, CGuard& guard, vector<IModel*>& Coins,int &coinsMissed)
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
		if (SphereToBoxCD(pThief,door, areaLength)) {

			if (doorType == simple) {
				InteractionMessage->Draw("Press 'E' to open.", 565, 550);
				if (myEngine->KeyHit(Key_E))
				{
					doorState = DOOR_OPENING;
				}
			}

			if (doorType == ending && keyFound && levels.GetLevelNumber()<3) {
				InteractionMessage->Draw("Press 'E' to go to the next level.", 565, 550);
				if (myEngine->KeyHit(Key_E))
				{
					coinsMissed += Coins.size();
					levels.NextLevel(walls, doors, pillars, key, guard,Coins);
					keyFound = false;
					Vector Pos = levels.GetPlayerSPos();
					pThief->SetPosition(Pos.x, Pos.y, Pos.z + 40);
					pThief->LookAt(Pos.x, Pos.y, Pos.z + 41);
					pThief->Scale(5);
					ThiefState = WAITING;

					for (int i = 0; i < doors.size(); i++) {
						if (doors[i].type == starting) {
							doors[i].state = DOOR_OPENING;
							break;
						}
					}
				}
			}
			if(doorType == ending && levels.GetLevelNumber()>=3)
			{
				InteractionMessage->Draw("Press 'E' to EXIT", 565, 550);
				if (myEngine->KeyHit(Key_E))
				{		
					coinsMissed += Coins.size();
					finished = true;
					STATE = END;
				}
			}

			if (doorType == ending && !keyFound) {
				InteractionMessage->Draw("You have to find the key first.", 565, 550);
			}

			if (doorType == starting) {
				InteractionMessage->Draw("No turning back now.", 565, 550);
			}
		}
		
	}break;
	case DOOR_OPEN:
	{
		if (SphereToBoxCD(pThief, door, areaLength)) {
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
		door->MoveLocalZ(-doorMovementSpeed);
		currentLimit += 0.1;
		if (currentLimit > maxLimit)
		{
			doorState = DOOR_CLOSED;
			currentLimit = 0;
		}

	}break;
	case DOOR_OPENING:
	{
		door->MoveLocalZ(doorMovementSpeed);
		currentLimit += 0.1;
		if (currentLimit > maxLimit)
		{
			doorState = DOOR_OPEN;
			currentLimit = 0;
			if (doorType == starting)
				ThiefState = FORWARD;
		}
	}break;
	}
}
void CollisionToHandleDoors(IModel* pThief, vector<DoorStruct>& door, IFont* InteractionMessage, I3DEngine* myEngine, float dt, bool& keyFound, CLevel& levels, vector<WallStruct>& walls, vector<DoorStruct>& doors, vector<PillarStruct>& pillars, IModel*& key, int& ThiefState, int& STATE, bool& finished, CGuard& guard, vector<IModel*>& Coins,int &coinsMissed) {
	for (int i = 0; i < door.size(); i++) {
		UpdateDoor(door[i].state, door[i].model, door[i].MaxDoorLimit, door[i].CurrentDoorLimit, InteractionMessage, myEngine, door[i].movementSpeed, keyFound, door[i].type, pThief, door[i].areaLength, levels, walls, doors, pillars, key, ThiefState, STATE, finished, guard,Coins,coinsMissed);
	}
}
void KeyCollision(IModel* pThief, float R1, float R2, CLevel level, bool& keyFound, IModel*& key) {

	if (!keyFound) {
		float x = pThief->GetX() - key->GetX();
		float y = pThief->GetY() - key->GetY();
		float z = pThief->GetZ() - key->GetZ();

		if (sqrt(x * x + y * y + z * z) < R1 + R2) {
			keyFound = true;
			level.RemoveKey(key);
		}
	}		
}
void reloadLevel(I3DEngine* myEngine,int &STATE,bool &keyFound,int &score,IModel* pThief,CLevel &levels, vector<DoorStruct>& doors,IModel*& key) {
		STATE = LEVEL;
		keyFound = false;
		score = 0;
		pThief->SetPosition(levels.GetPlayerSPos().x, levels.GetPlayerSPos().y, levels.GetPlayerSPos().z);
		pThief->LookAt(levels.GetPlayerSPos().x, levels.GetPlayerSPos().y, levels.GetPlayerSPos().z + 1);
		pThief->Scale(5);
		for (int i = 0; i < doors.size(); i++) {
			if (doors[i].state == DOOR_OPEN)
				doors[i].state = DOOR_CLOSING;
		}
		levels.ReloadKey(key);
}
void restartGame(int& STATE, bool& keyFound, int& score, CLevel& levels, vector<DoorStruct>& doors, vector<PillarStruct>& pillars, vector<WallStruct>& walls, IModel*& key, IModel*& pThief, CGuard& guard, vector<IModel*>& coins)
{
	STATE = LEVEL;
	keyFound = false;
	score = 0;
	levels.Restart(walls, doors, pillars, key, guard,coins);
	pThief->SetPosition(levels.GetPlayerSPos().x, levels.GetPlayerSPos().y, levels.GetPlayerSPos().z);
	pThief->LookAt(levels.GetPlayerSPos().x, levels.GetPlayerSPos().y, levels.GetPlayerSPos().z + 1);
	pThief->Scale(5);
}
void updateCoins(IModel* pThief, float R1, float R3, CLevel level, vector<IModel*>& coins,int &score) {

	for (int i = 0; i < coins.size(); i++)
	{
		coins[i]->RotateY(2); //rotation of all coins

		float x = pThief->GetX() - coins[i]->GetX();
		float y = pThief->GetY() - coins[i]->GetY();
		float z = pThief->GetZ() - coins[i]->GetZ();

		if (sqrt(x * x + y * y + z * z) < R1 + R3) {
			level.RemoveCoin(coins[i]);
			coins.erase(coins.begin() + i);
			score += 10;
		}
	}
	
}
void updateScore(IFont* Score,int &score) {
	stringstream StringScore;
	StringScore << score;
	Score->Draw("Score: " + StringScore.str(), 1150, 0);
}
void gameSettings(I3DEngine* myEngine, bool& pause, IFont* DisplayBigMessage, IFont* ControlsMessage)
{
	//exit
	if (myEngine->KeyHit(Key_Escape))myEngine->Stop(); 

	//pause functionalities
	if (myEngine->KeyHit(Key_P))pause = !pause;

	if (pause)
	{
		DisplayBigMessage->Draw("PAUSED", 500, 300);
		ControlsMessage->Draw("'T' to restart game", 1100, 0);
		ControlsMessage->Draw("'R' to restart\n current level", 1100, 20);
		ControlsMessage->Draw("'ESC' to exit", 1100, 70);

	}
}
void main()
{
	//Seed rand
	srand(time(0));
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
	CGuard guard(myEngine);

	//Model Containers
	vector<WallStruct> walls;
	vector<DoorStruct> doors;
	vector<PillarStruct> pillars;
	vector<IModel*> coins;
	
	IModel* key=0;

	int STATE = MENU;
	levels.NextLevel(walls, doors,pillars,key,guard,coins);

	//IMPORTANT VARIABLES
	float dt=myEngine->Timer();
	float thiefMovementSpeed = 5;

	//Create Thief
	IMesh* pThieflMesh = myEngine->LoadMesh("thief.x");
	IModel* pThief = pThieflMesh->CreateModel(levels.GetPlayerSPos().x,levels.GetPlayerSPos().y,levels.GetPlayerSPos().z);
	pThief->Scale(5);
	int ThiefState = NORMAL;
	ICamera* camera = myEngine->CreateCamera(kManual,0,2,-2);
	camera->RotateX(25);
	//when thief dies variables
	float currentRotation = 0;
	float maxRotation = 80;
	float rotationSpeed = 100;

	//IFONT Variables
	IFont* DisplayBigMessage = myEngine->LoadFont("Cambria", 120U);
	IFont* DisplayMenu = myEngine->LoadFont("Cambria", 70U);
	//Message Displaying variables
	IFont* DisplayQuest = myEngine->LoadFont("Cambria", 24U);
	IFont* InteractionMessage = myEngine->LoadFont("Cambria", 24U);
	IFont* ControlsMessage = myEngine->LoadFont("Cambria", 24U);
	IFont* Score = myEngine->LoadFont("Cambria", 24U);
	float currentTime = 0;
	float const maxTimer = 10;
	//END OF IFONT Variables
	
	//Camera variables 
	SCameraVariables CameraV;
	IMesh* dummyOnCameraMesh = myEngine->LoadMesh("dummy.x");
	CameraV.dummyOnCamera = dummyOnCameraMesh->CreateModel();
	CameraV.dummyOnCamera->Scale(5);
	CameraV.dummyOnCamera->AttachToParent(camera);

	camera->AttachToParent(pCameraDummy);
	pCameraDummy->AttachToParent(pThief);
	pCameraDummy->RotateY(180);

	//Key related variables
	bool keyFound = false;

	//Key variables
	float R1 = 5;
	float R2 = 7;
	float R3 = 3; //coin
	float keyMovementSpeed = 150;

	//Scoring variables
	int score = 0;
	string ratings[] = { "A+","A","A-","B+","B","B-","C+","C","C-","D+","D","D-","F" };
	int coinsMissed=0; //sums up together all coins that are missed when the player goes to the next level
	string finalRating = "A+";

	//Game varaibles
	bool pause = false;
	bool lost = false;
	bool finished = false;
	//END OF IMPORTANT VARIABLES 
	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// Draw the scene
		myEngine->DrawScene();
		dt = myEngine->Timer();
		/**** Update your scene each frame here ****/
		/*
		MAIN GAME SWITCH
		States: MENU, LEVEL, PLAYER_LOST, RELOAD_CURRENT_LEVEL, LOADING_NEXT_LEVEL

		MENU:---------------------------------------------------------
		1 Stops the mouse capture inserted by the player
		1 Messages displayed on the screen 
		2 Transitions to LEVEL after the player has hit Space
		END OF MENU:---------------------------------------------------

		LEVEL:---------------------------------------------------------
		4 Starts mouse capture that disables the mouse of the player to move around 
			and centers it in the middle of the screen
		5 Enables Collision between a larger scale of the doors and the player 
			to enable him to open or close a door
		6 Sphere to sphere collision between the player and the key
		7 Update the animation of the key
		8 Updates the movement of the model using WASD keys, also the player has the ability to
			run faster by holding Left Shift
		9 Captures the movement of the mouse and set some restrains some of the camera movement
			by using a dummyCamera attached to the player 
			(We dont want the camera to rotate without limits)
		10 Checks for collision detection between the thief and the objects that exists in the level 
			(pillars,walls,doors)
		11 Displays the Quests on the screen. Also displays the controls for a few seconds after 
			the player has started playing
		END OF LEVEL:--------------------------------------------------

		PLAYER_LOST:--------------------------------------------------
		//must fill later because we will add more things in the previous section
		PLAYER_LOST:--------------------------------------------------

		//fill the rest when the time comes 
		*/
		if (!pause) {
			switch (STATE)
			{
			case MENU:
			{	
				if (myEngine->KeyHit(Key_Space)) {
					DisplayBigMessage->Draw("LOADING . . .", 350, 300);
					STATE = LEVEL;
					myEngine->StartMouseCapture();// Disables mouse moving and centers it in the center of the screen 
				}
				else
				{
				DisplayBigMessage->Draw("Shadows & Gold", 300, 300);
				DisplayMenu->Draw("Hit Space To Start!", 420, 450);
				InteractionMessage->Draw("Hit 'D' for DEBUG MODE", 1000, 0);
				InteractionMessage->Draw("Find the Keys and Get all the Gold!", 500, 600);
				}

				if (myEngine->KeyHit(Key_D)) { 
					STATE = DEBUG_MODE;
					myEngine->StartMouseCapture();// Disables mouse moving and centers it in the center of the screen 
				}
			break;
			}
			case LEVEL:
			{
				cout << coinsMissed << endl;
				//Update			
				CollisionToHandleDoors(pThief, doors, InteractionMessage, myEngine, dt, keyFound, levels, walls, doors, pillars, key, ThiefState, STATE, finished, guard,coins,coinsMissed);		
				KeyCollision(pThief, R1, R2, levels, keyFound, key);		
				if (!keyFound)key->RotateY(keyMovementSpeed * dt);
				UpdateModel(myEngine, pThief, thiefMovementSpeed, dt, ThiefState, levels, doors);			
				UpdateCamera(myEngine, pThief, CameraV, pCameraDummy, camera, walls, ThiefState);			
				ThiefCollisionBehavior(myEngine, walls, pillars, doors, pThief, thiefMovementSpeed, dt);					
				UpdateMessages(keyFound, DisplayQuest, InteractionMessage, ControlsMessage, currentTime, maxTimer, dt, levels);//11
				CameraCollisionBehavior(camera, pThief, myEngine, walls, pillars, doors, CameraV, pCameraDummy, levels);
				updateCoins(pThief, R1, R3, levels, coins,score);				
				updateScore(Score, score);
				ThiefToGuardCD(pThief, guard.m_Model, STATE, lost);
				//guard.Update(dt, levels, myEngine, Vector(pThief->GetX(), 0, pThief->GetZ())); //Should keep guard Update near the end as it changes the dt when pathfinding occurs.

				if (myEngine->KeyHit(Key_R)) {
					STATE = RELOAD_CURRENT_LEVEL;
				}
				if (myEngine->KeyHit(Key_T))STATE = END;

				break;
			}
			case THIEF_DIES:
			{
				pThief->RotateLocalZ(rotationSpeed*dt);
				currentRotation += rotationSpeed * dt;
				if (currentRotation > maxRotation) {
					STATE = RELOAD_CURRENT_LEVEL;
				}

			}break;
			case RELOAD_CURRENT_LEVEL:
			{		
				if (lost) {
					DisplayBigMessage->Draw("You Lost!\n(-10 score)", 350, 300);
					DisplayMenu->Draw("Hit Space to Try Again!", 420, 600);
				}
				else
				{
					DisplayMenu->Draw("Are you sure you want to restart \nthe current level?\n(-10 score)\nHit Space to confirm", 50, 300);
					DisplayMenu->Draw("Hit 'B' to go back!", 420, 600);
				}
				if (myEngine->KeyHit(Key_Space))
				{
					reloadLevel(myEngine, STATE, keyFound, score, pThief, levels, doors, key);
					lost = false;
					score -= 10;
				}

				if (myEngine->KeyHit(Key_B))STATE = LEVEL;

			}
			break;
			case END:
			{
				if (finished)
				{
					stringstream StringScore;
					StringScore << score;

					stringstream StringCoinsMissed;
					StringCoinsMissed << coinsMissed;

					DisplayBigMessage->Draw("Congratulations you WON!", 100, 300);
					InteractionMessage->Draw("Score:"+ StringScore.str()+"\nRating : "+finalRating+"\nCoins Missed:"+ StringCoinsMissed.str(),100, 500);
					DisplayMenu->Draw("Hit Space to Try Again!", 400, 0);
				}
				else
				{
					DisplayMenu->Draw("Hit Space to Restart Game!", 350, 450);
					DisplayMenu->Draw("Hit 'B' to go back!", 420,550);
				}

				if (myEngine->KeyHit(Key_Space))
					restartGame(STATE, keyFound, score, levels, doors, pillars, walls, key, pThief, guard, coins);

				if (myEngine->KeyHit(Key_B))STATE = LEVEL;

			}break;
			case DEBUG_MODE:
			{
				/*DEBUG MODE:
				THIEF COLLISION WITH WALLS DISABLED
				THIEF COLLISION WITH PILLARS DISABLED
				THIEF COLLISION WITH DOORS DISABLED
				CAMERA COLLISION BEHAVIOR DISABLED
				GUARD BEHAVIOR DISABLED
				PRESS Q TO LOAD NEXT LEVEL ENABLED
				*/
				InteractionMessage->Draw("Hit 'Q' to load next level", 900, 0);	
				KeyCollision(pThief, R1, R2, levels, keyFound, key);		
				if (!keyFound)key->RotateY(keyMovementSpeed * dt); 
				UpdateModel(myEngine, pThief, thiefMovementSpeed, dt, ThiefState, levels, doors);			
				UpdateCamera(myEngine, pThief, CameraV, pCameraDummy, camera, walls, ThiefState);			
				UpdateMessages(keyFound, DisplayQuest, InteractionMessage, ControlsMessage, currentTime, maxTimer, dt, levels);
				CollisionToHandleDoors(pThief, doors, InteractionMessage, myEngine, dt, keyFound, levels, walls, doors, pillars, key, ThiefState, STATE, finished, guard,coins, coinsMissed);
				updateCoins(pThief, R1, R3, levels, coins, score);

				//go to the next level after p is hit
				if (myEngine->KeyHit(Key_Q))
					levels.NextLevel(walls, doors, pillars, key, guard,coins);
				break;

			}
			}
		}

		gameSettings(myEngine, pause,DisplayBigMessage,ControlsMessage);

	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}