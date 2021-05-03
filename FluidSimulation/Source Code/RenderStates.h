#pragma once

#include "Algebra3.h"

class SimObj;
class Camera;

struct RenderStates
{
	bool isAlive;//self explanatory
	vec3 pos;//position information
	vec3 vel;//velocity information
	vec3 angles;//angle information
	float scale;//how big?
	SimObj *linkedSimObj;
};
