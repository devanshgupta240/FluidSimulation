#include "Simulation.h"
#include "SimObj.h"

Sim::Sim()
{
	curSimObjs = new simVector();
	nextSimObjs = new simVector();
}

void Sim::simulateScene(double dt)
{
	SimObj *simObj;
	for(simVector::iterator simItem = curSimObjs->begin(); simItem != curSimObjs->end(); simItem++)
	{
		simObj = *simItem;
		if(simObj->update(dt))
		{
			nextSimObjs->push_back(simObj);
		}
		else
		{
			delete simObj;
		}
	}

	simVector *temp = curSimObjs;
	curSimObjs = nextSimObjs;
	nextSimObjs = temp;
	nextSimObjs->clear();
}

void Sim::addToSim(SimObj *simObj)
{
	nextSimObjs->push_back(simObj);
}

Sim::~Sim()
{
	if(curSimObjs)
		delete curSimObjs;
	if(nextSimObjs)
		delete nextSimObjs;
}