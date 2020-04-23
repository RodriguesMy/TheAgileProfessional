#pragma once
#include "Definitions.h"

bool CollisionSTS(Vector V1, Vector V2, float radius);
bool CollisionSTS(IModel* model1, IModel* model2, float radius);
bool SphereToBoxCD(IModel* model1, IModel* model2, Vector areaLength);