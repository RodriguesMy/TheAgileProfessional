#include <deque>
#include "Definitions.h"
#pragma once
using namespace tle;

class CLevel;
class CGuard
{
private:
	enum EState {
		patrol,
		hunt
	};
	struct SNode
	{
		int x;
		int y;
		int Dijkstra = 0;
		int score = 0;
		SNode* parent = 0;
		SNode(int X = 0, int Y = 0) {
			x = X;
			y = Y;
		}
	};
	IMesh* m_Mesh;
	IModel* m_Model;
	vector<Vector> m_Patrol; //container holding all positions the guard will be going towards
	vector<vector<int>> m_Grid; // A top side view of the entire level scaled so each element of the 2d dynamic array repressents 5 units, used to find the path the guard will take
	deque<SNode*> m_Path; //the path the guard takes
	float m_PathTime;
	EState m_State;
	float m_HuntingTime;

	//constants
	const float m_Speed = 4;
	const float m_HuntingTimeMax = 5;
	const float m_ViewAngle = 80;
	const float m_ViewDistance = 40;
public:
	CGuard(I3DEngine* myEngine);
	void CreatePoint(Vector point);//add a point to the guards patrol
	void ClearPoints();//delete all points of the guards patrol
	void SetPosition();//move the guard to that position
	void Update(float& dt, CLevel Level, I3DEngine*& myEngine, Vector Thief);//Update the guard, meaning decide what behaviour he should have and where to go
	void SetGrid(vector<vector<int>> grid) { m_Grid = grid; }//setter of the grid
	IModel* GetModel() { return m_Model; }//getter
private:
	void FindPath(Vector Target, CLevel Level);//pathfinding algorithm
	void GetChildNode(SNode* pCurrent, deque<SNode*>& open, deque<SNode*> closed, int x, int y, int Targetx, int Targety);//adds nodes that are valid to the open list
	bool CheckNode(SNode* pChild, deque<SNode*> open, deque<SNode*> closed);//checks if it is valid and doesnt exist in the open or closed lists
	bool CheckNode(SNode* pChild);//checks if it is valid but doesn check for its existence in the open or closed lists
	bool Move(float dt, CLevel Level);//move along the path
	float CatMullRom(float p1, float p2, float p3, float p4, float t);//calculation for a smoother path
	void ClearPath();//delete all snodes from the path
	bool InSight(Vector Target, CLevel Level);//check the angle and the distance of the guard and the target also if there is an obstacle between them
};