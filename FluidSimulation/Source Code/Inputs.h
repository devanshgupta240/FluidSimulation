#pragma once

#include <stdlib.h>

class SimObj;

class Input
{
private:
	static SimObj *simObj;
	static int oldX;
	static int oldY;

public:
	static bool buttonLocked;
	static void handleKeyPress(unsigned char key, int x, int y);
	static void handleMouseMotion(int x, int y);
	static void handleMouseClick(int button, int state, int x, int y);
	static void bindObjToInput(SimObj *simObject);
};
