#include <TL-Engine.h>
#include <deque>
#include "Definitions.h"
#pragma once
using namespace tle;

class CLevel;
class CGuard
{
private:
	struct SNode
	{
		int x;            // x coordinate
		int y;            // y coordinate
		int Dijkstra = 0;	// used in more complex algorithms
		int score = 0;	// used in more complex algorithms
		SNode* parent = 0;// note use of raw pointer here
	};
	IMesh* m_Mesh;
	
	vector<Vector> m_Patrol; //container holding all positions the guard will be going towards
	int m_TargetIt; //number indicating the which elemnt in the m_Patrol holds position the guard is going toward now/except if he is hunting the thief
	vector<vector<int>> m_Grid; // A top side view of the entire level scaled so each element of the 2d dynamic array repressents 5 units, used to find the path the guard will take
	deque<SNode*> m_Path;
	float m_PathTime;
public:
	IModel* m_Model;
	CGuard(I3DEngine* myEngine);
	void CreatePoint(Vector point);
	void ClearPoints();
	void SetPosition();
	void SetGrid(vector<vector<int>> grid) { m_Grid = grid; }
	void Update(float& dt,CLevel Level, I3DEngine* myEngine);
private:
	bool FindPath(Vector target, CLevel Level);
	void GetChildNode(SNode* pCurrent, deque<SNode*>& open, deque<SNode*> closed, int x, int y, Vector target);
	bool CheckChild(SNode* pChild, deque<SNode*> open, deque<SNode*> closed);
	bool Move(float dt, CLevel Level);
	float CatMullRom(float p1, float p2, float p3, float p4, float t);
	void ClearPath();
};