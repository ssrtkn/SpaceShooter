#include "Laser.h"

Laser::Laser(string name, shared_ptr<Material> mat, shared_ptr<Material> mat2,shared_ptr<Mesh> mesh):name(name)
{
	mySM = SceneManager::getInstance();

	nodeBeam = mySM->createSceneNode("nodeBeam" + name); 

	entityBeam = mySM->createEntity("entityBeam" + name);
	nodeBeam->attachObject(entityBeam);
	entityBeam->setMaterial(mat);
	entityBeam->setMesh(mesh);

	nodeBeam2 = mySM->createSceneNode("nodeBeam2" + name);
	entityBeam2 = mySM->createEntity("entityBeam2" + name);
	nodeBeam2->attachObject(entityBeam2);
	entityBeam2->setMaterial(mat);
	entityBeam2->setMesh(mesh);

	nodeBeam->setPosition(XMVectorSet(-0.3f, -0.5f, 50.0f, 1));
	nodeBeam2->setPosition(XMVectorSet(0.3f, -0.5f, 50.0f, 1));
	nodeBeam->setScaling(XMVectorSet(0.02f, 0.02f, 100.0f, 0.0f));
	nodeBeam2->setScaling(XMVectorSet(0.02f, 0.02f, 100.0f, 0.0f));

	entityTarget = mySM->createEntity("entityTarget" + name);
	nodeTarget = mySM->createSceneNode("nodeTarget" + name);
	nodeTarget->attachObject(entityTarget);
	entityTarget->setMaterial(mat2);
	entityTarget->setMesh(mesh);

	nodeTarget->setPosition(XMVectorSet(0.0f, 0.0f, 16.0f, 0.0f));
	nodeTarget->setScaling(XMVectorSet(0.06f, 0.06f, 0.06f, 1));

	firing = false;
	shared_ptr<SceneNode> cm = mySM->getSceneNode("CamNode2");
	cm->addChild(nodeTarget);
}

Laser::~Laser(void)
{

}




void Laser::orbitX(int factor) 
{ 

}

void Laser::orbitY(int factor) 
{ 

}

bool Laser::isFiring() { return firing; }

void Laser::toggle() { 
	firing = !firing;
	shared_ptr<SceneNode> cm = mySM->getSceneNode("CamNode2");


	if (firing) {

		cm->addChild(nodeBeam);
		cm->addChild(nodeBeam2);
	}
	else {
		cm->removeChild("nodeBeam");
		cm->removeChild("nodeBeam2");
	}
}

void Laser::laserOn() {
	firing = true;
	shared_ptr<SceneNode> cm = mySM->getSceneNode("CamNode2");

		cm->addChild(nodeBeam);
		cm->addChild(nodeBeam2);

}

void Laser::laserOff() {
	firing = false;
	shared_ptr<SceneNode> cm = mySM->getSceneNode("CamNode2");

	cm->removeChild("nodeBeam" + name);
	cm->removeChild("nodeBeam2" + name);

}
