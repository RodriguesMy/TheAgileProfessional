#pragma once

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <iostream>
#include"Level.h"
using namespace tle;

//GAME STATES
#define MENU 1
#define LEVEL 2
#define PLAYER_LOST 3
#define LOADING_NEXT_LEVEL 4
#define RELOAD_CURRENT_LEVEL 5
#define DEBUG_MODE 6

//Thief States
#define WAITING 0
#define FORWARD 1
#define NORMAL 2

struct SCameraVariables {
	IModel* dummyOnCamera;
	//Rotation of camera variables
	float const maxCameraRotation = 25;
	float cameraAngle = 25;
	float const minCameraRotation = 10;

	//Camera distance from camera to player
	float maxCameraDistance = -1.5;
	float minCameraDistance = 2;
	float currentCameraDistance = -1.5;
};