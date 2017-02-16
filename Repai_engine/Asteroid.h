#pragma once
#include "Render.h"
using namespace Render;
using namespace std;

class Asteroid
{
private:


	bool destroyed;
	SceneManager *mySM;
	shared_ptr<Entity> entity;
	shared_ptr<SceneNode> node;

public:
	Asteroid(string name, shared_ptr<Material> mat, shared_ptr<Mesh> mesh);
	~Asteroid(void);
	string name;
	void advance();
	bool isDestroyed();
	void destroy();
	void setPos(XMVECTOR pos);
	string getName();
	shared_ptr<SceneNode>  getNode();

};

