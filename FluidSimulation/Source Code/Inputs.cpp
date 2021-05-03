#include "Inputs.h"
#include "SimObj.h"
#include "Scene.h"
#include "Camera.h"

SimObj* Input::simObj = NULL;
int Input::oldX = -1;
int Input::oldY = -1;
bool Input::buttonLocked = false;

void Input::handleKeyPress(unsigned char key, int x, int y)
{
	//Scene::keyResponse(key, x, y);

	if(simObj)
		simObj->respondToKey(key, x, y);
}

void Input::handleMouseMotion(int x, int y)
{
	//Scene::mouseMotionResponse(x, y, oldX, oldY);

	if(simObj)
		simObj->respondToMotion(x, y, oldX, oldY);

	if(!buttonLocked)
	{
		oldX = x;
		oldY = y;
	}

}

void Input::handleMouseClick(int button, int state, int x, int y)
{
	//Scene::mouseClickResponse(button, state, x, y);

	if(!buttonLocked) {
		oldX = x;
		oldY = y;
	}

	if(simObj)
		simObj->respondToClick(button, state, x, y);

}

void Input::bindObjToInput(SimObj *simObject)
{
	simObj = simObject;
}