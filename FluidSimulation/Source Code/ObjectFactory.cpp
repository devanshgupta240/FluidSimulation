#include "ObjectFactory.h"

#include "Simulation.h"
#include "Scene.h"//include the sim and the scene
#include "Inputs.h"
#include "RenderStates.h"
#include "RenderObj.h"
#include "SimObj.h"

Scene* ObjectFactory::scene = NULL;
Sim* ObjectFactory::sim = NULL;

void ObjectFactory::initFactory(Sim *gameSim, Scene *gameScene)
{
	sim = gameSim;
	scene = gameScene;
}

void ObjectFactory::MakeFluid()
{
	RenderStates *renderStates = new RenderStates();
	renderStates->isAlive = true;
	renderStates->pos = vec3(0,0);
	renderStates->vel = vec3(0,0,0);
	renderStates->angles = vec3(0,0,0);
	renderStates->scale = 1.0f;
	SimFluid *simFluid = new SimFluid(renderStates, 30, 30);
	renderStates->linkedSimObj = simFluid;
	Input::bindObjToInput(simFluid);
	RenderFluid *renderFluid = new RenderFluid(renderStates);
	add(simFluid, renderFluid);
}

void ObjectFactory::MakeObj()
{//not really useful
	RenderStates *renderStates = new RenderStates();
	renderStates->isAlive = true;
	renderStates->pos = vec3(0,0,0);
	renderStates->vel = vec3(0,0,0);
	renderStates->angles = vec3(0,0,0);
	renderStates->scale = 1.0f;
	SimObj *simObj = new SimObj(renderStates);
	RenderObj *renderObj = new RenderObj(renderStates);
	add(simObj, renderObj);
}

void ObjectFactory::MakeObj(vec3 pos, float scale)
{//not really useful
	RenderStates *renderStates = new RenderStates();
	renderStates->isAlive = true;
	renderStates->pos = pos;
	renderStates->vel = vec3(0,0,0);
	renderStates->angles = vec3(0,0,0);
	renderStates->scale = scale;
	SimObj *simObj = new SimObj(renderStates);
	RenderObj *renderObj = new RenderObj(renderStates);
	add(simObj, renderObj);
}


void ObjectFactory::add(SimObj *simObj, RenderObj *renderObj)
{
	sim->addToSim(simObj);
	scene->addToScene(renderObj);
}
