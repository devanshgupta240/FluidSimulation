#pragma once

#include <vector>
#include <string>
#include <stdlib.h>

#include <GL/glut.h>

class RenderObj;
class Camera;

typedef std::vector<RenderObj*> renderVector;

const int maxLights = 6;
const unsigned int envmap_dim = 128;

class Scene
{
private:
	renderVector* curRenderObjs;
	renderVector* nextRenderObjs;
	
	static Camera* cam;
	static bool allowMovement;
	static bool allowZoom;

	void updateLights();
	void initLights();
	void initGL();

	void renderObjects();

public:
	Scene();
	void renderScene();
	void addToScene(RenderObj *renderObj);
	Camera* getCamera();
	~Scene();
};
