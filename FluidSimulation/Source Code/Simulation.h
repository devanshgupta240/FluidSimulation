#pragma once

#include <vector>

class SimObj;

typedef std::vector<SimObj*> simVector;

class Sim
{
private:
	simVector *curSimObjs;
	simVector *nextSimObjs;
public:
	Sim();
	void simulateScene(double dt);
	void addToSim(SimObj *simObj);
	~Sim();
};
