#include "CGuard.h"
#include "Level.h"

CGuard::CGuard(I3DEngine* myEngine) {
	m_Mesh = myEngine->LoadMesh("Guard.x");
	m_Model = m_Mesh->CreateModel();
	m_Model->Scale(5);
	m_PathTime = 0;
	m_State = patrol;
	m_HuntingTime = 0;
}

void CGuard::CreatePoint(Vector point) {
	m_Patrol.push_back(point);
}

void CGuard::ClearPoints() {
	while (!m_Patrol.empty())
		m_Patrol.pop_back();
	ClearPath();
}

void CGuard::ClearPath() {
	while (!m_Path.empty()) {
		delete m_Path.back();
		m_Path.pop_back();
	}
}

void CGuard::SetPosition() {
	if (!m_Patrol.empty()) {
		int num = rand() % m_Patrol.size();
		m_Model->SetPosition(m_Patrol[num].x, 0, m_Patrol[num].z);
	}
}

bool CGuard::CheckChild(SNode* pChild, deque<SNode*> open, deque<SNode*> closed) {
	//The first two if check if the node is in the bounds of the grid
	if (pChild->y >= 0 && pChild->y < m_Grid.size()) {
		if (pChild->x >= 0 && pChild->x < m_Grid.at(pChild->y).size()) {
			if (m_Grid.at(pChild->y).at(pChild->x) != 0)//checks if the node is valid (not 0)
			{
				//checks if the node exists either in the closed or in the open containers
				for (int i = 0; i < closed.size(); i++)
					if (closed[i]->x == pChild->x && closed[i]->y == pChild->y)
						return false;
				for (int i = 0; i < open.size(); i++)
					if (open[i]->x == pChild->x && open[i]->y == pChild->y)
						return false;
				return true;
			}
		}
	}
	//didnt use an else so to avoid having 3 else statements all returning false for the first three if statements
	return false;
}

void CGuard::GetChildNode(SNode* pCurrent, deque<SNode*>& open, deque<SNode*> closed,int x,int y, int Targetx, int Targety){
	SNode* pChild = new SNode;
	pChild->x = x;
	pChild->y = y;
	if (CheckChild(pChild, open, closed)) {
		pChild->parent = pCurrent;
		pChild->Dijkstra = pCurrent->Dijkstra + 1;
		pChild->score = pChild->Dijkstra + (Targetx - x + Targety - y);
		open.push_back(pChild);
	}
}

void CGuard::FindPath(Vector Target, CLevel Level) {
	ClearPath();
	SNode* pCurrent = new SNode;
	float Minx = Level.GetMin().x;
	float Minz = Level.GetMin().z;
	int NormalisedTargetx = (Target.x - Minx) / 5;
	int NormalisedTargetz = (Target.z - Minz) / 5;
	deque<SNode*> open;
	deque<SNode*> closed;
	pCurrent->x = (m_Model->GetX() - Minx) / 5;
	pCurrent->y = (m_Model->GetZ() - Minz) / 5;
	open.push_front(pCurrent);
	while (!open.empty()){
		pCurrent = open.front();
		open.pop_front();
		//check if we reached the target
		if (pCurrent->x == NormalisedTargetx && pCurrent->y == NormalisedTargetz) {
			//construct the path
			while (pCurrent) {
				SNode* pTemp = pCurrent->parent;
				m_Path.push_front(pCurrent);
				pCurrent = pTemp;
			}
			break;
		}
		else{
			//generate all children of the current node and check if they are valid and havent already been generated
			GetChildNode(pCurrent, open, closed, pCurrent->x, pCurrent->y + 1, NormalisedTargetx, NormalisedTargetz);
			GetChildNode(pCurrent, open, closed, pCurrent->x + 1, pCurrent->y, NormalisedTargetx, NormalisedTargetz);
			GetChildNode(pCurrent, open, closed, pCurrent->x, pCurrent->y - 1, NormalisedTargetx, NormalisedTargetz);
			GetChildNode(pCurrent, open, closed, pCurrent->x - 1, pCurrent->y, NormalisedTargetx, NormalisedTargetz);
			//sort by score to find the next best node
			for (int i = 0; i+1 < open.size(); i++) {
				for (int j = i + 1; j < open.size(); j++) {
					if (open[i]->score > open[j]->score) {
						iter_swap(open.begin() + i, open.begin() + j);
					}
				}
			}
			closed.push_back(pCurrent);
		}
	}
	// if we get out of the while loop then we cant find a path to the target and the path container remains empty
}

bool CGuard::Move(float dt, CLevel Level) {
	m_PathTime += dt;
	if (m_PathTime + 1.0f >= m_Path.size()) {
		m_PathTime = 0;
		return true;
	}
	else {
		int p2 = static_cast<int>(m_PathTime);
		int p3 = p2 + 1;
		int p4 = p3 + 1;
		if (p4 == m_Path.size()) {
			p4--;
		}
		int p1 = p2 - 1;
		if (p1 < 0) {
			p1 = 0;
		}
		float t = fmodf(m_PathTime, 1.0f);
		float Minx = Level.GetMin().x;
		float Minz = Level.GetMin().z;
		float x = CatMullRom((m_Path[p1]->x * 5.0f) + Minx, (m_Path[p2]->x * 5.0f) + Minx, (m_Path[p3]->x * 5.0f) + Minx, (m_Path[p4]->x * 5.0f) + Minx, t);
		float y = CatMullRom((m_Path[p1]->y * 5.0f) + Minz, (m_Path[p2]->y * 5.0f) + Minz, (m_Path[p3]->y * 5.0f) + Minz, (m_Path[p4]->y * 5.0f) + Minz, t);

		m_Model->LookAt(x, 0, y);
		m_Model->SetPosition(x, 0, y);
		m_Model->Scale(5);
		return false;
	}
}
float CGuard::CatMullRom(float p1, float p2, float p3, float p4, float t) {
	float a = -p1 + 3 * p2 - 3 * p3 + p4;
	float b = 2 * p1 - 5 * p2 + 4 * p3 - p4;
	float c = p3 - p1;
	float d = 2 * p2;

	float output = (a * t * t * t + b * t * t + c * t + d) / 2;
	return output;
}

void CGuard::Update(float& dt, CLevel Level, I3DEngine*& myEngine, Vector Thief) {
	switch (m_State)
	{
	case patrol:
		if (Move(dt, Level)) {
			if (!m_Patrol.empty()) {
				FindPath(m_Patrol.at(rand() % m_Patrol.size()), Level);
				dt = myEngine->Timer();
			}
		}
		if (InSight(Thief)) {
			m_HuntingTime = 0;
			ClearPath();
			m_State = hunt;
		}
		break;
	case hunt:
		m_HuntingTime += dt;
		cout << m_HuntingTime << " " << dt << endl;
		if (Move(dt, Level)) {
			FindPath(Thief, Level);
			dt = myEngine->Timer();
		}
		if (m_HuntingTime > m_HuntingTimeMax) {
			FindPath(Thief, Level);
			m_HuntingTime = 0;
			dt = myEngine->Timer();
			if (!InSight(Thief)) {
				ClearPath();
				m_State = patrol;
			}
		}
		break;
	}
}

bool CGuard::InSight(Vector Target) {
	Vector guardToTarget = Vector(Target.x - m_Model->GetX(), 0, Target.z - m_Model->GetZ());
	float distance = sqrt(guardToTarget.x * guardToTarget.x + guardToTarget.z * guardToTarget.z);
	float matrix[16];
	m_Model->GetMatrix(matrix);
	Vector Forward = Vector(matrix[8], matrix[9], matrix[10]);
	float crossProd = Forward.x * guardToTarget.x + Forward.z * guardToTarget.z;
	crossProd /= (sqrt(Forward.x * Forward.x + Forward.z * Forward.z) * distance);
	return acos(crossProd) * 180 / 3.142 <= m_ViewAngle && distance < m_ViewDistance;
}