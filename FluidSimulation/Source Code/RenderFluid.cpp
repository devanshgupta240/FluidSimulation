#include "RenderFluid.h"

#include "SimFluid.h"

RenderFluid::RenderFluid(RenderStates* renderInfo)
{
	renderStates = renderInfo;
	simFluid = (SimFluid*)renderStates->linkedSimObj;
	vertices = new GLfloat[(simFluid->dimX)*(simFluid->dimY)*6];
	glGenTextures(1,&densityTex);
	colors = new GLfloat[(simFluid->dimX)*(simFluid->dimY)*6];
	densityColors = new GLfloat[(simFluid->dimX)*(simFluid->dimY)*3];
	densityVertices = new GLfloat[(simFluid->dimX)*(simFluid->dimY)*3];
	densityIndices = new GLuint[(simFluid->dimX-1)*(simFluid->dimY-1)*4];
	initVertices();
	initDensityIndices();
	for (int i = 0; i < (simFluid->dimX)*(simFluid->dimY)*6; i=i+6)
	{
		colors[i] = 0.0;
		colors[i+1] = 0.0;
		colors[i+2] = 1.0;
		colors[i+3] = 1.0;
		colors[i+4] = 0.0;
		colors[i+5] = 0.0;
	}
	//cout << "done with color init" << endl;

}

RenderFluid::RenderFluid(void)
{
	vertices = colors = NULL;
	densityTex = 0;
}

bool RenderFluid::render()
{
	/* init vertex arrays */
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDisable(GL_LIGHTING);
	
	updateVertices(); /*copy updated velocityGrid values over*/

	glPushMatrix();
		glTranslatef(renderStates->pos[VX], renderStates->pos[VY], renderStates->pos[VZ]);

		glVertexPointer(3, GL_FLOAT, 0, vertices);
		if(simFluid->displayType % 3 == 1 || simFluid->displayType % 3 == 2)
		{
			glColorPointer(3, GL_FLOAT, 0, colors);
			glDrawArrays(GL_LINES, 0, (simFluid->dimX)*(simFluid->dimY)*2);
		}

		glDisableClientState(GL_COLOR_ARRAY);

		if(simFluid->displayForces)
		{
			glColor3f(0,1,0);

			updateForces(); //switch the lines displayed by the velocity vertices to show forces
			glDrawArrays(GL_LINES, 0, (simFluid->dimX)*(simFluid->dimY)*2);
		}

		glVertexPointer(3, GL_FLOAT, 0, densityVertices);
		if(simFluid->displayType % 3 == 0 || simFluid->displayType % 3 == 2)
		{
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(3, GL_FLOAT, 0, densityColors);
		}
		else
			glColor3f(0,0,0);//just paint it black

		glDrawElements(GL_QUADS, (simFluid->dimX-1)*(simFluid->dimY-1)*4, GL_UNSIGNED_INT, densityIndices);


	glPopMatrix();
	
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	//cout << "DONE WITH RENDER()" << endl;
	return renderStates->isAlive;

}

void RenderFluid::initVertices()
{
	int vi = 0;
	for(int i = 1; i <= simFluid->dimX; i++)
	{
		for(int j = 1; j <= simFluid->dimY; j++)
		{
			densityVertices[vi/2] = vertices[vi] = simFluid->getPointFromGridLoc(i,j)[VX];
			densityVertices[vi/2 + 1] = vertices[vi+1] = simFluid->getPointFromGridLoc(i,j)[VY];
			densityVertices[vi/2 + 2] = vertices[vi+2] = 0.0;
			vi += 6;
		}		
	}
}

void RenderFluid::initDensityIndices()
{
	int index = 0;
	for(int i = 0; i < simFluid->dimX - 1; i++)
	{
		for(int j = 0; j < simFluid->dimY - 1; j++)
		{
			int vi = i + j * simFluid->dimX;
			densityIndices[index] = vi;
			densityIndices[index+1] = vi + 1;
			densityIndices[index+2] = vi + simFluid->dimX + 1;
			densityIndices[index+3] = vi + simFluid->dimX;
 			index += 4;
		}
	}
}

void RenderFluid::clampToEdge(int vi)
{
	vec3 lowerLeft = simFluid->getPointFromGridLoc(1,1);
	vec3 upperRight = simFluid->getPointFromGridLoc(simFluid->dimX,simFluid->dimY);

	if(vertices[vi + 3] > upperRight[VX])//keep in bounds
		vertices[vi + 3] = upperRight[VX];
	if(vertices[vi + 3] < lowerLeft[VX])
		vertices[vi + 3] = lowerLeft[VX];

	if(vertices[vi + 4] > upperRight[VY])//keep in bounds
		vertices[vi + 4] = upperRight[VY];
	if(vertices[vi + 4] < lowerLeft[VY])
		vertices[vi + 4] = lowerLeft[VY];
}

void RenderFluid::updateVertices()
{
	int vi = 0;
	for(int i = 1; i <= simFluid->dimX; i++)
	{
		for(int j = 1; j <= simFluid->dimY; j++)
		{
			vec3 density = simFluid->densityGrid[simFluid->index(i,j)];
			densityColors[vi/2] = min(1.0, density[VX]);
			densityColors[vi/2 + 1] = min(1.0, density[VY]);
			densityColors[vi/2 + 2] = min(1.0, density[VZ]);

			vertices[vi + 3] = vertices[vi] + simFluid->velocityGrid[simFluid->index(i,j)][VX];
			vertices[vi + 4] = vertices[vi + 1] + simFluid->velocityGrid[simFluid->index(i,j)][VY];
			vertices[vi + 5] = 0.0;
			clampToEdge(vi);

			vi += 6;
		}
	}
}

void RenderFluid::updateForces()
{
	// vec3 lowerLeft = simFluid->getPointFromGridLoc(1,1);
	// vec3 upperRight = simFluid->getPointFromGridLoc(simFluid->dimX,simFluid->dimY);
	int vi = 0;
	for(int i = 1; i <= simFluid->dimX; i++)
	{
		for(int j = 1; j <= simFluid->dimY; j++)
		{
			double scale = 0.02;

			vertices[vi+3] = vertices[vi] + scale*simFluid->forcesGrid[simFluid->index(i,j)][VX];
			vertices[vi+4] = vertices[vi+1] + scale*simFluid->forcesGrid[simFluid->index(i,j)][VY];
			vertices[vi + 5] = 0.0;
			clampToEdge(vi);

			vi += 6;
		}
	}
}

RenderFluid::~RenderFluid(void)
{
	if(vertices)
		delete vertices;
	if(colors)
		delete colors;
	if(densityVertices)
		delete densityVertices;
	if(densityColors)
		delete densityColors;
	if(densityIndices)
		delete densityIndices;
}
