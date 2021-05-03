#include "Scene.h"
#include "RenderObj.h"
#include "Camera.h"

Camera* Scene::cam = NULL;
bool Scene::allowMovement = true;
bool Scene::allowZoom = true;

Scene::Scene()
{
	initGL();

	curRenderObjs = new renderVector();
	nextRenderObjs = new renderVector();

	cam = new Camera();
}

void Scene::initGL()
{
	//gl initialization
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glClearColor(0.75,0.75,0.75,1);

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	initLights();
}

void Scene::updateLights()
{
        GLfloat light0_position[] = {0, 0, 1, 0};//light 0 is the overhead light
        glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
}

void Scene::initLights()
{
        glEnable(GL_LIGHTING);

        //shading and lighting model
        glShadeModel(GL_SMOOTH);
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

        //turn on the lights and set their color
        glEnable(GL_LIGHT0);
        GLfloat ambient_color[] = {0.15, 0.15, 0.15, 0.0};
        GLfloat diff0_color[] = {0.85, 0.85, 0.85, 1.0};
        GLfloat highlight0_color[] = {1, 1, 1, 1.0};
        glLightfv(GL_LIGHT0, GL_SPECULAR, highlight0_color);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diff0_color);
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_color);
}

void Scene::renderScene()
{
	glMatrixMode(GL_MODELVIEW);
	
	glPushMatrix();
		glLoadIdentity();

		vec3 camPos = cam->getPos();
		vec3 camUp = cam->getUp();
		vec3 camCenter = vec3(0);//just a viewer for now
		gluLookAt(camPos[VX], camPos[VY], camPos[VZ], camCenter[VX], camCenter[VY], camCenter[VZ], camUp[VX], camUp[VY], camUp[VZ]);//look according to the camera

		renderObjects();
	glPopMatrix();

	glutSwapBuffers();

	glFlush();
}

void Scene::renderObjects()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	RenderObj *renderObj;

	for(renderVector::iterator renderItem = curRenderObjs->begin(); renderItem != curRenderObjs->end(); renderItem++)
	{
		renderObj = *renderItem;
		if(renderObj->render())//draw it
		{
			nextRenderObjs->push_back(renderObj);//we're keeping it, so put it in for the next frame
		}
		else
		{
			delete renderObj;//we're not keeping it, kill it so we don't leak
		}
	}
	renderVector *temp = curRenderObjs;
	curRenderObjs = nextRenderObjs;
	nextRenderObjs = temp;
	nextRenderObjs->clear();//swap the vectors around and clear out the next list

}

void Scene::addToScene(RenderObj *renderObj)
{
	nextRenderObjs->push_back(renderObj);
}

Camera* Scene::getCamera()
{
	return cam;
}

Scene::~Scene()
{
	if(curRenderObjs)
		delete curRenderObjs;
	if(nextRenderObjs)
		delete nextRenderObjs;
	if(cam)
		delete cam;
}