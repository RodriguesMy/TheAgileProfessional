#pragma once

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <iostream>
#include <vector>
#include <random>
using namespace tle;

//GAME STATES
#define MENU 1
#define LEVEL 2
#define RELOAD_CURRENT_LEVEL 3
#define DEBUG_MODE 4
#define END 5
#define THIEF_DIES 6

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

struct Vector {
	float x;
	float y;
	float z;
	Vector() {
		x = 0;
		y = 0;
		z = 0;
	}
	Vector(float X, float Y, float Z) {
		this->x = X;
		this->y = Y;
		this->z = Z;
	}
};