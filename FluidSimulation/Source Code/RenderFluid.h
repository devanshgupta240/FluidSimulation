#pragma once
#include "RenderObj.h"
#include "SimFluid.h"
class RenderFluid :
	public RenderObj
{
private:
	GLfloat* vertices;
	GLfloat* colors;
	GLfloat* densityColors;
	GLfloat* densityVertices;
	GLuint* densityIndices;
	SimFluid* simFluid;
	GLuint densityTex;
	void clampToEdge(int vi);
	void initVertices(); /* initialize vertices with positions of velocity grid cells */
	void initDensityIndices();
	void updateVertices();
	void updateForces();

public:
	RenderFluid(RenderStates *renderStates);
	RenderFluid(void);

	bool render();

	~RenderFluid(void);
};

