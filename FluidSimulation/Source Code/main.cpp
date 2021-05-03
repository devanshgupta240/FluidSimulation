#include <stdio.h>
#include <iostream>
#include <sys/timeb.h>

#include <GL/glut.h>

#include "ObjectFactory.h"
#include "Simulation.h"
#include "Scene.h"
#include "Inputs.h"
#include "SimFluid.h"

using namespace std;

const int frameRate = 60;

Scene *scene;
Sim *sim;
int prevFrameTime = 1000;

//init functions
void init();

//functions called by glut
void renderScene();
void simulate();//this tells the simulation to simulate
void reshape(int width, int height);
void mouseMotion(int x, int y);
void mouseClicks(int button, int state, int x, int y);
void registerKey(unsigned char key, int x, int y);

//time functions
int getMilliCount();
int getMilliSpan(int nTimeStart);

int main(int argc, char** argv)
{
	argc = 1;
	glutInit(&argc, argv);//initialize glut
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(640, 480);
	glutCreateWindow("Fluid Simulation");//give us a window
	//cout << "---------------------before init" << endl;
	init();//initialize the rest of the program
	//cout << "---------------------after init" << endl;
    glutMouseFunc(mouseClicks);
    glutMotionFunc(mouseMotion);
	glutKeyboardFunc(registerKey);//set up the handlers for user input, these will be dealt with here in main
	glutDisplayFunc(renderScene);
	glutIdleFunc(simulate);
	glutReshapeFunc(reshape);
	//cout << "---------------------entering main loop" << endl;
	glutMainLoop();
	return 0;
}

int getMilliCount()
{
	timeb tb;
	ftime( &tb );
	int nCount = tb.millitm + (tb.time & 0xfffff) * 1000;
	return nCount;
}

int getMilliSpan(int nTimeStart)
{
	int nSpan = getMilliCount() - nTimeStart;
	if ( nSpan < 0 )
	nSpan += 0x100000 * 1000;
	return nSpan;
}

void init()
{
	sim = new Sim();
	scene = new Scene();
	ObjectFactory::initFactory(sim, scene);

	ObjectFactory::MakeFluid();
	//ObjectFactory::MakeTeapot();
	//ObjectFactory::MakeObj(vec3(0,0,5),1);
	//ObjectFactory::MakeObj(vec3(0,0,-4),4);
}

void renderScene()
{
	//cout << "---------------------before renderScene" << endl;
	scene->renderScene();
	//cout << "---------------------after renderScene" << endl;
}

void simulate()
{
	if(getMilliSpan(prevFrameTime) >= 1000/frameRate)
	{
		prevFrameTime = getMilliCount();
		sim->simulateScene(1/double(frameRate));
		glutPostRedisplay();
	}
	//cout << "---------------------exiting simulate" << endl;
}

void reshape(int width, int height)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, width/(float)height, 0.1, 99);
	glViewport(0, 0, width, height);
}

void registerKey(unsigned char key, int x, int y)
{
	Input::handleKeyPress(key, x, y);
}

void mouseMotion(int x, int y)
{
    Input::handleMouseMotion(x, y);
}

void mouseClicks(int button, int state, int x, int y)
{
    Input::handleMouseClick(button, state, x, y);
}
