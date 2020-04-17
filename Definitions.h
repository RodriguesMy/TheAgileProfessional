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

struct SCameraVariables {
	//Rotation of camera variables
	float const maxCameraRotation = 35;
	float cameraAngle = 25;
	float const minCameraRotation = 10;

	//Camera distance from camera to player
	float maxCameraDistance = -2;
	float minCameraDistance = 0;
	float currentCameraDistance = -2;
};