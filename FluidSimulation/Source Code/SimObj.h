#pragma once

struct RenderStates;

class SimObj
{
protected:
	RenderStates *renderStates;
public:
	SimObj();
	SimObj(RenderStates *renderInfo);
	virtual void respondToKey(unsigned char key, int x, int y);
	virtual void respondToMotion(int x, int y, int oldX, int oldY);
	virtual void respondToClick(int button, int state, int x, int y);
	virtual bool update(double dt);
	~SimObj();
};

#include "SimFluid.h"
//include all the sim objects here so we can catch all of them by including SimObj
