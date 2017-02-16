#pragma once
#include "Render.h"
using namespace Render;
using namespace std;

const float LASER_TURN_SPEED = 0.025f;

class Laser
{
private:
	SceneManager *mySM;

	shared_ptr<Entity> entityBeam;
	shared_ptr<Entity> entityBeam2;
	shared_ptr<Entity> entityTarget;
	shared_ptr<SceneNode> nodeBeam;
	shared_ptr<SceneNode> nodeBeam2;
	shared_ptr<SceneNode> nodeTarget;

	bool firing;
	float vertAngle;
	float horzAngle;

	string name;

public:
	Laser(string name, shared_ptr<Material> mat, shared_ptr<Material> mat2,shared_ptr<Mesh> mesh);
	~Laser(void);

	void orbitX(int);
	void orbitY(int);
	void toggle();
	void laserOn();
	void laserOff();
	bool isFiring();
};

