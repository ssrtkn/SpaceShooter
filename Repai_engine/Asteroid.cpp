#include "Asteroid.h"


Asteroid::Asteroid(string name, shared_ptr<Material> mat, shared_ptr<Mesh> mesh):name(name)
{

	mySM = SceneManager::getInstance();
	entity = mySM->createEntity("entity"+name);
	node = mySM->createSceneNode("node"+name);
	node->attachObject(entity);
	entity->setMaterial(mat);
	entity->setMesh(mesh);
	destroyed = false;
}


Asteroid::~Asteroid(void)
{

}

shared_ptr<SceneNode>  Asteroid::getNode() { return node; }

void Asteroid::setPos(XMVECTOR pos) {
	node->setPosition(pos);
}

void Asteroid::advance()
{
}

bool Asteroid::isDestroyed() {
	return destroyed;
}
string Asteroid::getName() {
	return name;
}
void Asteroid::destroy() {
	shared_ptr<SceneNode> m = mySM->getSceneNode("MoonNode");
	m->removeChild("node" + name);
	destroyed = true;
}

