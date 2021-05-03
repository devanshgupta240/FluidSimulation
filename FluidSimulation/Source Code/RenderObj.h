#pragma once

#include <stdlib.h>

#include <GL/glut.h>
#include "RenderStates.h"

struct RenderStates;

class RenderObj
{
protected:
	RenderStates *renderStates;
public:
	RenderObj();
	RenderObj(RenderStates *renderInfo);
	virtual bool render();
	~RenderObj();
};

#include "RenderFluid.h"
