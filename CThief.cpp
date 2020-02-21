#include "CThief.h"
#include <TL-Engine.h>	// TL-Engine include file and namespace
using namespace tle;

//Setters
void CThief::setMesh(IMesh* mesh)
{
	this->mThiefMesh = mesh;
}
void CThief::setModel(IModel* model)
{
	this->mThiefModel = model;
}
void  CThief::setMovementSpeed(int movementSpeed)
{
	this->mMovementSpeed = movementSpeed;
}

//Getters
IMesh* CThief::getMesh()
{
	return this->mThiefMesh;
}
IModel* CThief::getModel()
{
	return this->mThiefModel;
}
int CThief::getMovementSpeed()
{
	return this->mMovementSpeed;
}

//Constructors/Destructors
CThief::CThief(IMesh* mesh,IModel* model, int movementSpeed,I3DEngine* myEngine)
{
	setMesh(mesh);
	setModel(model);
	setMovementSpeed(movementSpeed);

	CThief(myEngine);

}
CThief::CThief(I3DEngine* myEngine)
{
	IMesh* thiefMesh = myEngine->LoadMesh("sierra.x");
	IModel* thief = thiefMesh->CreateModel(0, 0, 0);

}
CThief::CThief()
{

}
CThief::~CThief()
{

}
