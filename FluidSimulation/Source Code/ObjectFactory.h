#pragma once

#include "Algebra3.h"

class Sim;
class SimObj;
class Scene;
class RenderObj;

class ObjectFactory
{
private:
	static Sim *sim;
	static Scene *scene;
	static void add(SimObj *simObj, RenderObj *renderObj);
public:
	static void MakeObj();
	static void MakeObj(vec3 pos, float scale);
	// static void MakeTeapot();
	static void MakeFluid();
	static void initFactory(Sim *gameSim, Scene *gameScene);
};
