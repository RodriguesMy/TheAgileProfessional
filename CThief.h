#pragma once
#include <TL-Engine.h>	// TL-Engine include file and namespace
using namespace tle;

class CThief
{
private:
	//Member Data
	IMesh* mThiefMesh;
	IModel* mThiefModel;
	int mMovementSpeed;
public:
	//Member Methods

	//Setters
	void setMesh(IMesh* mesh);
	void setModel(IModel* model);
	void setMovementSpeed(int movementSpeed);

	//Getters
	IMesh* getMesh();
	IModel* getModel();
	int getMovementSpeed();

	//Constructors/Destructors
	CThief(IMesh* mesh, IModel* model, int movementSpeed, I3DEngine* myEngine);
	CThief(I3DEngine* myEngine);
	CThief();
	~CThief();
};
