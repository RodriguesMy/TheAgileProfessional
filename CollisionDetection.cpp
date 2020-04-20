#include "CollisionDetection.h"

bool CollisionSTS(Vector V1, Vector V2, float radius) {
	return radius > sqrt(pow((V1.x - V2.x), 2) + pow((V1.y - V2.y), 2) + pow((V1.z - V2.z), 2));
}

bool SphereToBoxCD(IModel* model1, IModel* model2, Vector areaLength)
{
	return ((model1->GetX() < model2->GetX() + areaLength.x && model1->GetX() > model2->GetX() - areaLength.x &&
		model1->GetY() < model2->GetY() + areaLength.y && model1->GetY() > model2->GetY() - areaLength.y &&
		model1->GetZ() < model2->GetZ() + areaLength.z && model1->GetZ() > model2->GetZ() - areaLength.z));
}