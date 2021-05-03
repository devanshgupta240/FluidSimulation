#include "SimObj.h"
#include "ObjectFactory.h"//so we can access the object factory
#include "RenderStates.h"

SimObj::SimObj()
{
}

SimObj::SimObj(RenderStates *renderInfo)
{
	renderStates = renderInfo;
}

bool SimObj::update(double dt)
{
	return renderStates->isAlive;
}

void SimObj::respondToKey(unsigned char key, int x, int y)
{
	return;//do nothing by default
}

void SimObj::respondToMotion(int x, int y, int oldX, int oldY)
{
	return;//do nothing by default
}

void SimObj::respondToClick(int button, int state, int x, int y)
{
	return;//do nothing by default
}

SimObj::~SimObj()
{
}