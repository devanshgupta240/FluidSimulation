#include "SimFluid.h"
#include "RenderStates.h"

#include "Camera.h"
#include "RenderStates.h"
#include "Inputs.h"

const vec3 red = vec3(1,0,0);
const vec3 green = vec3(0,1,0);
const vec3 blue = vec3(0,0,1);
const vec3 white = vec3(1,1,1);
const vec3 yellow = vec3(1,1,0);
const vec3 magenta = vec3(1,0,1);
const vec3 cyan = vec3(0,1,1);

SimFluid::SimFluid()
{
	dimType = 2; /* 2D sim by default */
	displayType = 0;
	dimX = 0;
	dimY = 0;
	brushsize = 0;
	curColor = white;
	displayForces = true;
	allowAddForce = false;
	allowAddDensity = false;
	renderStates = NULL;
	velocityGrid = NULL;
	nextVelocityGrid = NULL;
	densitySourceGrid = NULL;
	densityGrid = NULL;
	nextDensityGrid = NULL;
	forcesGrid = NULL;
	densitySourceGrid = NULL;
	divergence = NULL;
	q = NULL;
	objX = 0.0;
	objY = 0.0;
	objZ = 0.0;
	modelMat = NULL;
	projMat = NULL;
	viewport = NULL;
}

SimFluid::SimFluid(RenderStates *renderInfo, int width, int height)
{
	dimType = 2; /* 2D sim by default */
	dimX = width;
	dimY = height;
	renderStates = renderInfo;
	velocityGrid = new vec2[(dimX + 2) * (dimY + 2)];
	nextVelocityGrid = new vec2[(dimX + 2) * (dimY + 2)];
	forcesGrid = new vec2[(dimX + 2) * (dimY + 2)];
	densityGrid = new vec3[(dimX + 2) * (dimY + 2)];
	nextDensityGrid = new vec3[(dimX + 2) * (dimY + 2)];
	densitySourceGrid = new vec3[(dimX + 2) * (dimY + 2)];
	divergence = new double[(dimX + 2) * (dimY + 2)];
	q = new double[(dimX + 2) * (dimY + 2)];//q is the scalar field in the Hodge decomposition fo the velocity grid
	
	reset();

	objX = 0.0;
	objY = 0.0;
	objZ = 0.0;
	modelMat = new GLdouble[16];
	projMat = new GLdouble[16];
	viewport = new GLint[4];;
}

/*Update methods*/

bool SimFluid::update(double dt)
{
	velocityStep(dt);
	densityStep(dt);
	return renderStates->isAlive;
}

/*Utility Functions*/

void SimFluid::reset()
{
	displayType = 0;
	displayForces = true;
	allowAddForce = false;
	allowAddDensity = false;
	curColor = white;
	diffRate = diffIncrement = 0.001;
	cout << "Diffusion Rate: " << diffRate << endl;
	brushsize = 0;
	cout << "Brush Size: " << brushsize + 1 << endl;

	for (int i = 0; i < dimX + 2; i++) {
		for (int j = 0; j < dimY + 2; j++) {
			velocityGrid[index(i,j)] = vec2(0.0, 0.0);
			nextVelocityGrid[index(i,j)] = vec2(0.0,0.0); //initialize to 0
			forcesGrid[index(i,j)] = vec2(0.0,0.0);
			densityGrid[index(i,j)] = 0.0;
			nextDensityGrid[index(i,j)] = 0.0;
			densitySourceGrid[index(i,j)] = 0.0;
		}
	}
	setBoundaryConditions();
}

int SimFluid::index(int i, int j)
{
	return i * (dimY + 2) + j;
}

vec2 SimFluid::getPointFromGridLoc(int i, int j)
{
	return vec2((double)i, (double)j) - getPointToGridOffset();//minus because we're going from the grid to the point
}

vec2 SimFluid::getPointFromGridLoc(int combinedIndex)
{
	int i = combinedIndex / (dimY + 2);
	int j = combinedIndex % (dimY + 2);
	return getPointFromGridLoc(i, j);
}

int SimFluid::index(vec2 point)
{
	point += getPointToGridOffset();
	int i = floor(point[VX] + 0.5);//round to nearest int
	if (i > dimX + 2)
		i = dimX + 2;
	if (i < 0)
		i = 0;
	int j = floor(point[VY] + 0.5);//round to nearest int
	if (j < 0)
		j = 0;
	if (j > dimY + 2)
		j = dimY + 2;
	return index(i, j);
}

bool SimFluid::inBounds(int index)
{
	int i = index / (dimY + 2);//row
	int j = index % (dimY + 2);//column
	return i >= 0 && i <= dimX + 1 && j >= 0 && j <= dimY + 1;//if any of these conditions are violated, the index is out of bounds
}

vec2 SimFluid::getPointToGridOffset()
{
	return vec2(1 + 0.5 * (1 - (dimX % 2)) + dimX/2 - (1 - dimX % 2), 1 + 0.5 * (1 - (dimY % 2)) + dimY/2 - (1 - dimY % 2));//1-dimX%2 is 1 if dimX is even, 0 if odd, we add 1 to address the extra cells used for boundary conditions
}

vec2 SimFluid::traceParticle(vec2 startPoint, double deltaT, double subStepSize)
{
	vec2 endPoint = startPoint;
	
	while(fabs(deltaT) > fabs(subStepSize))
	{//while there's enough time left for a full substep
		deltaT -= subStepSize;//reduce the remaining deltaT
		endPoint = traceParticle(endPoint, subStepSize);//update the approximation
	}

	return traceParticle(endPoint, deltaT);//trace out whatever time remains
}

vec2 SimFluid::traceParticle(vec2 startPoint, double deltaT)
{//uses the 4th order runge kutta method as described here: http://graphics.cs.ucdavis.edu/~joy/ecs277/other-notes/Numerical-Methods-for-Particle-Tracing-in-Vector-Fields.pdf
	vec2 firstApproxOffset = deltaT * interpolateVels(startPoint);//k1
	vec2 secondApproxOffset = deltaT * interpolateVels(startPoint + 0.5 * firstApproxOffset);//k2
	vec2 thirdApproxOffset = deltaT * interpolateVels(startPoint + 0.5 * secondApproxOffset);//k3
	vec2 fourthApproxOffset = deltaT * interpolateVels(startPoint + thirdApproxOffset);//k4

	return startPoint + (1.0/6.0) * (firstApproxOffset + 2 * secondApproxOffset + 2 * thirdApproxOffset + fourthApproxOffset);
}

/*Density Methods*/

void SimFluid::densityStep(double timeStep)
{
	addDensities(timeStep);
	diffuseDensities(timeStep);
	advectDensities(timeStep, timeStep/10.0);
}

void SimFluid::swapDensityGrids()
{
	vec3* temp = densityGrid;
	densityGrid = nextDensityGrid;
	nextDensityGrid = temp;//swap the pointers
}

void SimFluid::addDensities(double timeStep)
{
	for(int i = 1; i <= dimX; i++)
		for(int j = 1; j <= dimY; j++)
		{
			nextDensityGrid[index(i, j)] = densityGrid[index(i, j)] + densitySourceGrid[index(i, j)];
			if(!allowAddDensity)
				densitySourceGrid[index(i, j)] = 0.0;
		}
	swapDensityGrids();
}

void SimFluid::diffuseDensities(double timeStep)
{//this code is heavily based on the gauss-seidel relaxation code in the Jos Stam's paper RT Fluid Dynamics for Games since that's pretty much the most compact way to do it
	double diffusionRate = diffRate * timeStep * dimX * dimY;
	
	//we need to solve the system given by backtracing the diffusion to the previous time step:
	//densityGrid[i,j] = nextDensityGrid[i-1,j] - diffusionRate * (nextDensityGrid[i+1,j] + nextDensityGrid[i,j-1] + nextDensityGrid[i,j+1] + nextDensityGrid[i,j] - 4*nextDensityGrid[i,j])
	//in matrix form this is Ax = b, x is the nextDensityGrid(the unknown new time step), b is the densityGrid(time step we're backwards diffusing to from x) and A is a very sparse matrix with just 5 entries per row(one for each of the four adjacent cells and and the i,j cell) 

	for(int i = 0; i < dimX + 2; i++)
		for(int j = 0; j < dimY + 2; j++)
			nextDensityGrid[index(i,j)] = densityGrid[index(i,j)];//this sets it up so our starting guess for the value of the x vector is b(shouldn't be too far from the truth)

	for(int k = 0; k < 50; k++)
	{//control loop for the number of iterations
		for(int i = 1; i <= dimX; i++)
		{
			for(int j = 1; j <= dimY; j++)
			{
				//this performs the iterative update for gauss seidel relaxation, the equation for which can be found here: http://mathworld.wolfram.com/Gauss-SeidelMethod.html
				nextDensityGrid[index(i, j)] = (densityGrid[index(i, j)] //b[i]
					+ diffusionRate * (nextDensityGrid[index(i - 1, j)] + nextDensityGrid[index(i + 1, j)] + nextDensityGrid[index(i, j - 1)] + nextDensityGrid[index(i, j + 1)]))//the two sums(the notation required to express them is impossible in a text comment) in the update equation
						/ (1 + 4 * diffusionRate);//entry A[i,i]
			}
		}
		setDensityBoundaryConditions();//reset the boundary conditions to keep them from skewing the values on the border back to their old values
	}
	swapDensityGrids();
}

void SimFluid::advectDensities(double timeStep, double subTimeStep)
{
	for(int i = 1; i <= dimX; i++)
	{
		for(int j = 1; j <= dimY; j++)
		{
			vec2 point = getPointFromGridLoc(i, j);
			vec2 endPoint = traceParticle(point, -timeStep, -subTimeStep);
			//cout << index(point) << endl;
			vec3 newDensity = interpolateDensities(endPoint);
			nextDensityGrid[index(i,j)] = newDensity;
		}
	}

	//cout << "advect finished" << endl;

	setBoundaryConditions();

	swapDensityGrids();//swap the velocity grids
}

vec3 SimFluid::interpolateDensities(vec2 point)
{//assumes the point is in bounds or on the boundary, blows up if the point is completely out of bounds, uses bilinear filtering if not on the boundary
	vec2 nearLowerLeft = point + vec2(-0.5, -0.5);//the four corners that bound the point we're interpolating to
	vec2 nearUpperLeft = point + vec2(-0.5, 0.5);
	vec2 nearLowerRight = point + vec2(0.5, -0.5);
	vec2 nearUpperRight = point + vec2(0.5, 0.5);
	
	if(!(inBounds(index(nearLowerLeft)) && inBounds(index(nearLowerRight)) && inBounds(index(nearUpperLeft)) && inBounds(index(nearUpperRight))))
		return densityGrid[index(point)];//the point must be on the boundary, so don't interpolate

	vec3 lowerLeftDens = densityGrid[index(nearLowerLeft)];
	vec3 upperLeftDens = densityGrid[index(nearUpperLeft)];
	vec3 lowerRightDens = densityGrid[index(nearLowerRight)];
	vec3 upperRightDens = densityGrid[index(nearUpperRight)];//densities in each corner

	vec2 lowerLeft = getPointFromGridLoc(index(nearLowerLeft));

	double u = point[VX] - lowerLeft[VX];
	double v = point[VY] - lowerLeft[VY];

	vec3 densLower = (1 - u) * lowerLeftDens + u * lowerRightDens;
	vec3 densUpper = (1 - u) * upperLeftDens + u * upperRightDens;//horizontal interpolation

	return (1-v) * densLower + v * densUpper;//the final bilinearly interpolated density at the point
}

void SimFluid::setDensityBoundaryConditions()
{//anything that leaves the grid falls out of the sim
	for(int i = 1; i <= dimX; i++)
	{//vertical walls
		nextDensityGrid[index(i, 0)] = 0;
		nextDensityGrid[index(i, dimY + 1)] = 0;
	}

	for(int j = 1; j <= dimY; j++)
	{//horizontal walls
		nextDensityGrid[index(0, j)] = 0;
		nextDensityGrid[index(dimX + 1, j)] = 0;
	}

	//now handle the corners
	nextDensityGrid[index(0, 0)] = 0;
	nextDensityGrid[index(0, dimY + 1)] = 0;
	nextDensityGrid[index(dimX + 1, 0)] = 0;
	nextDensityGrid[index(dimX + 1, dimY + 1)] = 0;
}

/*Velocity Methods*/

void SimFluid::velocityStep(double timeStep)
{
	addForces(timeStep);
	diffuseVels(timeStep);
	project();
	advectVels(timeStep, timeStep/10.0);
	project();
}

void SimFluid::swapVelocityGrids()
{
	vec2* temp = velocityGrid;
	velocityGrid = nextVelocityGrid;
	nextVelocityGrid = temp;//swap the pointers
}

void SimFluid::addForces(double deltaT)
{
	for(int i = 1; i <= dimX; i++)
	{
		for(int j = 1; j <= dimY; j++)
		{
			nextVelocityGrid[index(i,j)] = velocityGrid[index(i,j)] + deltaT*forcesGrid[index(i,j)];
			if(!allowAddForce)
				forcesGrid[index(i,j)] = vec2(0.0);
		}
	}
	swapVelocityGrids();
}

void SimFluid::diffuseVels(double timeStep)
{//this code is heavily based on the gauss-seidel relaxation code in the Jos Stam's paper RT Fluid Dynamics for Games since that's pretty much the most compact way to do it
	double diffusionRate = diffRate * timeStep * dimX * dimY;
	
	//we need to solve the system given by backtracing the diffusion to the previous time step:
	//velocityGrid[i,j] = nextVelocityGrid[i-1,j] - diffusionRate * (nextVelocityGrid[i+1,j] + nextVelocityGrid[i,j-1] + nextVelocityGrid[i,j+1] + nextVelocityGrid[i,j] - 4*nextVelocityGrid[i,j])
	//in matrix form this is Ax = b, x is the nextVelocityGrid(the unknown new time step), b is the velocityGrid(time step we're backwards diffusing to from x) and A is a very sparse matrix with just 5 entries per row(one for each of the four adjacent cells and and the i,j cell) 

	for(int i = 0; i < dimX + 2; i++)
		for(int j = 0; j < dimY + 2; j++)
			nextVelocityGrid[index(i,j)] = velocityGrid[index(i,j)];//this sets it up so our starting guess for the value of the x vector is b(shouldn't be too far from the truth)

	for(int k = 0; k < 50; k++)
	{//control loop for the number of iterations
		for(int i = 1; i <= dimX; i++)
		{
			for(int j = 1; j <= dimY; j++)
			{
				//this performs the iterative update for gauss seidel relaxation, the equation for which can be found here: http://mathworld.wolfram.com/Gauss-SeidelMethod.html
				nextVelocityGrid[index(i, j)] = (velocityGrid[index(i, j)] //b[i]
					+ diffusionRate * (nextVelocityGrid[index(i - 1, j)] + nextVelocityGrid[index(i + 1, j)] + nextVelocityGrid[index(i, j - 1)] + nextVelocityGrid[index(i, j + 1)]))//the two sums(the notation required to express them is impossible in a text comment) in the update equation
						/ (1 + 4 * diffusionRate);//entry A[i,i]
			}
		}
		setBoundaryConditions();//reset the boundary conditions to keep them from skewing the values on the border back to their old values
	}
	swapVelocityGrids();
}

void SimFluid::project()
{//this will be very similar to the projection code used in Jos Stam's paper RT Fluid Dynamics for Games since that is what I based it on, as with diffuse, this is pretty much the simplest way to do it
	for(int i = 1; i <= dimX; i++)
	{
		for(int j = 1; j <= dimY; j++)
		{
			//using finite differencing to compute the divergence
			//not sure why the minus sign needs to be here, but this function goes horribly wrong without it
			divergence[index(i,j)] = -0.5 * (velocityGrid[index(i + 1,j)][VX] - velocityGrid[index(i - 1,j)][VX] + velocityGrid[index(i,j + 1)][VY] - velocityGrid[index(i,j - 1)][VY]);
			//initial guess for this is the 0 vector, we're going to run gaus seidel relaxation on it using spatial discretization of the poisson equation
			q[index(i, j)] = 0.0;
		}
	}

	for(int i = 0; i < dimX + 2; i++)
	{
		q[index(i, 0)] = 0.0;
		q[index(i, dimY + 1)] = 0.0;
	}
	
	for(int j = 0; j < dimY + 2; j++)
	{
		q[index(0, j)] = 0.0;
		q[index(dimX + 1, j)] = 0.0;
	}//more zero initializing

	//we're trying to solve (Laplacian of q) = (divergence of velocity grid)
	//this is a poisson equation so we're going to spatially discretize it and run gauss seidel relaxation
	for(int k = 0; k < 50; k++)
	{//gauss seidel relaxation to find q
		for(int i = 1; i <= dimX; i++)
		{
			for(int j = 1; j <= dimY; j++)
			{//gauss seidel relaxation update
				q[index(i, j)] = (divergence[index(i,j)] + q[index(i - 1, j)] + q[index(i + 1, j)] + q[index(i, j - 1)] + q[index(i, j + 1)])/4.0;//A[i,i] = 4.0, b = divergence
			}
		}
	}

	for(int i = 1; i <= dimX; i++)
	{
		for(int j = 1; j <= dimY; j++)
		{
			nextVelocityGrid[index(i,j)] = velocityGrid[index(i, j)] - 0.5 * vec2(q[index(i + 1, j)] - q[index(i - 1, j)], q[index(i, j + 1)] - q[index(i, j - 1)]);//subtract the gradient of q from the velocity grid
		}
	}

	setBoundaryConditions();
	swapVelocityGrids();//cleanup
}

void SimFluid::advectVels(double timeStep, double subTimeStep)
{
	for(int i = 1; i <= dimX; i++)
	{
		for(int j = 1; j <= dimY; j++)
		{
			vec2 point = getPointFromGridLoc(i, j);
			vec2 endPoint = traceParticle(point, -timeStep, -subTimeStep);
			//cout << index(point) << endl;
			vec2 newVel = interpolateVels(endPoint);
			nextVelocityGrid[index(i,j)] = newVel;
		}
	}

	//cout << "advect finished" << endl;

	setBoundaryConditions();

	swapVelocityGrids();//swap the velocity grids
}

vec2 SimFluid::interpolateVels(vec2 point)
{//assumes the point is in bounds or on the boundary, blows up if the point is completely out of bounds, uses bilinear filtering if not on the boundary
	vec2 nearLowerLeft = point + vec2(-0.5, -0.5);//the four corners that bound the point we're interpolating to
	vec2 nearUpperLeft = point + vec2(-0.5, 0.5);
	vec2 nearLowerRight = point + vec2(0.5, -0.5);
	vec2 nearUpperRight = point + vec2(0.5, 0.5);
	
	if(!(inBounds(index(nearLowerLeft)) && inBounds(index(nearLowerRight)) && inBounds(index(nearUpperLeft)) && inBounds(index(nearUpperRight))))
		return velocityGrid[index(point)];//the point must be on the boundary, so don't interpolate

	vec2 lowerLeftVel = velocityGrid[index(nearLowerLeft)];
	vec2 upperLeftVel = velocityGrid[index(nearUpperLeft)];
	vec2 lowerRightVel = velocityGrid[index(nearLowerRight)];
	vec2 upperRightVel = velocityGrid[index(nearUpperRight)];//velocities in each corner

	vec2 lowerLeft = getPointFromGridLoc(index(nearLowerLeft));

	double u = point[VX] - lowerLeft[VX];
	double v = point[VY] - lowerLeft[VY];

	vec2 velLower = (1 - u) * lowerLeftVel + u * lowerRightVel;
	vec2 velUpper = (1 - u) * upperLeftVel + u * upperRightVel;//horizontal interpolation

	return (1-v) * velLower + v * velUpper;//the final bilinearly interpolated velocity at the point
}

void SimFluid::setBoundaryConditions()
{//velocity does not leave the grid
	for(int i = 1; i <= dimX; i++)
	{//handle vertical walls
		nextVelocityGrid[index(i,0)] = vec2(fabs(nextVelocityGrid[index(i, 1)][VX]), nextVelocityGrid[index(i, 1)][VY]);
		nextVelocityGrid[index(i,dimY + 1)] = vec2(-fabs(nextVelocityGrid[index(i, dimY)][VX]), nextVelocityGrid[index(i, dimY)][VY]);
	}

	for(int j = 1; j <= dimY; j++)
	{//handle horizontal walls
		nextVelocityGrid[index(0,j)] = vec2(nextVelocityGrid[index(1, j)][VX], fabs(nextVelocityGrid[index(1,j)][VY]));
		nextVelocityGrid[index(dimX + 1,j)] = vec2(nextVelocityGrid[index(dimX, j)][VX], -fabs(nextVelocityGrid[index(dimX,j)][VY]));
	}
	//now handle the corners
	nextVelocityGrid[index(0, 0)] = 0.5 * (nextVelocityGrid[index(1, 0)] + nextVelocityGrid[index(0, 1)]);
	nextVelocityGrid[index(0, dimY + 1)] = 0.5 * (nextVelocityGrid[index(1, dimY + 1)] + nextVelocityGrid[index(0, dimY)]);
	nextVelocityGrid[index(dimX + 1, 0)] = 0.5 * (nextVelocityGrid[index(dimX, 0)] + nextVelocityGrid[index(dimX + 1, 1)]);
	nextVelocityGrid[index(dimX + 1, dimY + 1)] = 0.5 * (nextVelocityGrid[index(dimX + 1, dimY)] + nextVelocityGrid[index(dimX, dimY + 1)]);
}

/*UI*/

vec2 SimFluid::unProject(int x, int y)
{
	glGetDoublev(GL_MODELVIEW_MATRIX, modelMat);
	glGetDoublev(GL_PROJECTION_MATRIX, projMat);
	glGetIntegerv(GL_VIEWPORT, viewport);

	gluUnProject(double(x), double(y), 0.5, modelMat, projMat, viewport, &objX, &objY, &objZ);
	vec3 dir = vec3(objX, objY, objZ);
	dir.normalize();
	double scale = fabs(Camera::getPos()[VZ]) / dir[VZ];
	dir *= scale;
	//cout << dir[VX] << ", " << dir[VY] << ", " << dir[VZ] << endl;
	return vec2(dir[VX], dir[VY]);
}

void SimFluid::respondToMotion(int x, int y, int oldX, int oldY)
{
	if(oldX != -1 && oldY != -1 && allowAddForce && (oldX != x || oldY != y))
    {
		vec2 gridCell = unProject(oldX, oldY);
		gridCell[VX] = - gridCell[VX];
		vec2 offset = unProject(x, y);
		offset[VX] = -offset[VX];
		vec2 forceVec = offset - gridCell;
		int radius = 0;
		bool circleFinished;
		auto applyCircularBrush = [&circleFinished, gridCell, forceVec, this] (vec2 point)
		{
			if(point.length() <= brushsize)
			{
				circleFinished = false;
				forcesGrid[index(gridCell + point)] = 50*forceVec; //store the force vector in the clicked cell cluster
			}
		};
		do
		{
			circleFinished = true;//we start by assuming the circle is complete
			for(int i = -radius; i <= radius; i++)
			{//some redundancy here but whatever
				applyCircularBrush(vec2(-(double)radius, (double)i));
				applyCircularBrush(vec2((double)radius, (double)i));
				applyCircularBrush(vec2((double) i, -(double)radius));
				applyCircularBrush(vec2((double) i, (double)radius));
			}
			radius++;//increase the search radius
		}while(!circleFinished);
    }

	if(allowAddDensity)
    {
		vec2 gridCell = unProject(x, y);
		gridCell[VX] = - gridCell[VX];
		//cout << "center: " << centerX << ", " << centerY << endl;
		int radius = 0;
		bool circleFinished;
		auto applyCircularBrush = [&circleFinished, gridCell, this] (vec2 point)
		{
			if(point.length() <= brushsize)
			{
				circleFinished = false;
				densitySourceGrid[index(gridCell + point)] = 0.5*curColor; //store the density addition in the clicked cell cluster
			}
		};
		do
		{
			circleFinished = true;//we start by assuming the circle is complete
			for(int i = -radius; i <= radius; i++)
			{//some redundancy here but whatever
				applyCircularBrush(vec2(-(double)radius, (double)i));
				applyCircularBrush(vec2((double)radius, (double)i));
				applyCircularBrush(vec2((double) i, -(double)radius));
				applyCircularBrush(vec2((double) i, (double)radius));
			}
			radius++;//increase the search radius
		}while(!circleFinished);
    } 
}

void SimFluid::respondToClick(int button, int state, int x, int y)
{
	if(state == GLUT_DOWN)
	{
		switch(button)
		{
		case GLUT_LEFT_BUTTON:
			Input::buttonLocked = true;
			allowAddForce = true;
			break;
		case GLUT_RIGHT_BUTTON:
			allowAddDensity = true;
			break;
		default: 
			break;
		}
	}
	else
	{
		switch(button)
		{
		case GLUT_LEFT_BUTTON:
			Input::buttonLocked = false;
			allowAddForce = false;
			break;
		case GLUT_RIGHT_BUTTON:
			allowAddDensity = false;
			break;
		default: 
			break;
		}
	}
	respondToMotion(x, y, x, y);
}

void SimFluid::respondToKey(unsigned char key, int x, int y)
{
	switch(key)
	{
	case 'w':
		curColor = white;
		break;
	case 'r':
		curColor = red;
		break;
	case 'g':
		curColor = green;
		break;
	case 'b':
		curColor = blue;
		break;
	case 'y':
		curColor = yellow;
		break;
	case 'c':
		curColor = cyan;
		break;
	case 'm':
		curColor = magenta;
		break;
	case 'q':
		reset();
		break;
	case 'f':
		displayForces = !displayForces;
		break;
	case '1':
		displayType = 0;
		break;
	case '2':
		displayType = 1;
		break;
	case '3':
		displayType = 2;
		break;
	case '[':
		if(brushsize > 0)
			brushsize -= 1;
		cout << "Brushsize: " << brushsize + 1 << endl;
		break;
	case ']':
		brushsize += 1;
		cout << "Brushsize: " << brushsize + 1 << endl;
		break;
	case '+':
		diffRate += diffIncrement;
		if(diffRate * 1.01 >= diffIncrement * 10)
			diffIncrement = diffRate;
		cout << "Diffusion Rate: " << diffRate << endl;
		break;
	case '-':
		if(diffRate * 0.99 <= diffIncrement)
			diffIncrement /= 10;
		diffRate -= diffIncrement;
		cout << "Diffusion Rate: " << diffRate << endl;
		break;
	default:
		break;
	}
}

SimFluid::~SimFluid()
{
	if(velocityGrid)
		delete velocityGrid;
	if(nextVelocityGrid)
		delete nextVelocityGrid;
	if(densityGrid)
		delete densityGrid;
	if(nextDensityGrid)
		delete nextDensityGrid;
	if(forcesGrid)
		delete forcesGrid;
	if(densitySourceGrid)
		delete densitySourceGrid;
	if(divergence)
		delete divergence;
	if(q)
		delete q;
	if(modelMat)
		delete modelMat;
	if(projMat)
		delete projMat;
	if(viewport)
		delete viewport;
}
