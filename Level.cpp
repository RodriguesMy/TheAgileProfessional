#include "Level.h"


CLevel::CLevel(I3DEngine* myEngine)
{
	m_MapIt = 0;
	m_MDoor = myEngine->LoadMesh("");
	m_MWall = myEngine->LoadMesh("");
}


CLevel::~CLevel()
{
}
