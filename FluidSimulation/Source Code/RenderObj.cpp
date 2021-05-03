#include "RenderObj.h"

RenderObj::RenderObj()
{
	renderStates = NULL;
}

RenderObj::RenderObj(RenderStates *renderInfo)
{
	renderStates = renderInfo;
}

bool RenderObj::render()
{
	
	GLfloat one[] = {0.8, 0.8, 0.8, 1};
	GLfloat small[] = {0.35, 0.35, 0.35, 1};
	GLfloat amb[] = {0.5, 0.5, 0.5, 1};

	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	glMaterialfv(GL_FRONT, GL_SPECULAR, one);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, small);
	glMaterialf(GL_FRONT, GL_SHININESS, 8);

	glPushMatrix();
		glTranslatef(renderStates->pos[VX], renderStates->pos[VY], renderStates->pos[VZ]);
		glutSolidSphere(renderStates->scale, 128, 64);
	glPopMatrix();
	return renderStates->isAlive;
}

RenderObj::~RenderObj()
{
	delete renderStates;
}