#pragma once
#include "SimObj.h"
#include "Algebra3.h"
#include <GL/glut.h>

class SimFluid :
	public SimObj
{
private:
	/*General Variables*/
	int dimType; /* 2 = 2D or 3 = 3D */
	int dimX;
	int dimY;
	int displayType;
	bool displayForces;
	double diffRate;
	double diffIncrement;
	/*Density Variables*/
	vec3 *densityGrid;
	vec3 *nextDensityGrid;
	vec3 *densitySourceGrid;
	/*Velocity Variables*/
	vec2 *velocityGrid;//keep extra cells on edges for boundary conditions
	vec2 *nextVelocityGrid;
	vec2 *forcesGrid;
	double *divergence;
	double *q;
	/*UI Variables*/
	int brushsize;
	vec3 curColor;
	bool allowAddDensity;
	bool allowAddForce;
	GLdouble objX;
	GLdouble objY;
	GLdouble objZ;
	GLdouble* modelMat;
	GLdouble* projMat;
	GLint* viewport;

	/*Utility Functions*/
	void reset();
	int index(int i, int j);
	int index(vec2 point);//point to index conversion
	bool inBounds(int index);
	vec2 traceParticle(vec2 startPoint, double deltaT, double subStepSize);
	vec2 traceParticle(vec2 startPoint, double deltaT);
	vec2 getPointToGridOffset();
	vec2 getPointFromGridLoc(int i, int j);
	vec2 getPointFromGridLoc(int combinedIndex);

	/*Velocity Step Functions*/
	void addForces(double deltaT);
	void diffuseVels(double timeStep);
	void advectVels(double timeStep, double subTimeStep);
	vec2 interpolateVels(vec2 point);
	void project();
	void velocityStep(double timeStep);
	void swapVelocityGrids();
	void setBoundaryConditions();
	void fadeForces();
	
	/*Density Step Functions*/
	void addDensities(double deltaT);
	void densityStep(double timeStep);
	void diffuseDensities(double timeStep);
	void advectDensities(double timeStep, double subTimeStep);
	vec3 interpolateDensities(vec2 point);
	void swapDensityGrids();
	void setDensityBoundaryConditions();

	/*UI Functions*/
	vec2 unProject(int x, int y);


public:
	friend class RenderFluid; /*RenderFluid needs access to SimFluid grids*/
	SimFluid(RenderStates *renderInfo, int width, int height);
	void respondToMotion(int x, int y, int oldX, int oldY);
	void respondToClick(int button, int state, int x, int y);
	void respondToKey(unsigned char key, int x, int y);
	bool update(double dt);

	SimFluid();
	~SimFluid();
};

