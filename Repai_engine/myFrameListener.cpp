
#include <WindowsX.h>

#include "myFrameListener.h"

using namespace Render;


myFrameListener::myFrameListener(std::string name) :
	FrameListener(name), mX(0), mY(0), dX(0), dY(0), mMouseDown(false), mLightMove(true), mKeyLEnabled(true)
{

	mySM = SceneManager::getInstance();
	assetM = AssetManager::getInstance();
	life = 3;
	score = 0;
	countAsteroid = NUM_DEBRIS;
	damageFrame = 0;
	initGame();

}

myFrameListener::~myFrameListener(void)
{
	delete[] debris;
}

void myFrameListener::initGame() {
	addLight();
	addCamera();
	addSkybox();
	addGameObject();

}

void myFrameListener::addLight() {
	shared_ptr<Light> dirlight = mySM->createLight("DirectionalLight");
	dirlight->setDirectional(XMFLOAT4(0.4f, 0.4f, 0.4f, 0.0f),
		XMFLOAT4(0.8f, 0.8f, 0.8f, 0.0f),
		XMFLOAT4(0.2f, 0.2f, 0.2f, 0.0f));
	shared_ptr<SceneNode> ln = mySM->createSceneNode("DirLightNode");
	mySM->getRootSceneNode()->addChild(ln);
	ln->attachObject(dirlight);
	ln->setPosition(XMVectorSet(200, 200, 0, 1));
	ln->lookAt(XMVectorSet(0, 0, 0, 1));
}

void myFrameListener::addSkybox() {
	shared_ptr<Entity> sky = mySM->createEntity("Sky", Entity::ET_SKY);
	shared_ptr<SceneNode> skynode = mySM->createSceneNode("SkyNode");
	mySM->getRootSceneNode()->addChild(skynode);
	skynode->attachObject(sky);
	matsky = assetM->createMaterial("Sky");
	matsky->setTexture(std::wstring(L"..\\..\\Textures\\orbital.dds"));
	matsky2 = assetM->createMaterial("Sky2");
	matsky2->setTexture(std::wstring(L"..\\..\\Textures\\orbital_damage.dds"));
	sky->setMaterial(matsky);

	shared_ptr<MeshSphere> skymesh = assetM->createMeshSphere("MeshSky", 10, 10);
	sky->setMesh(skymesh);
}

void myFrameListener::addCamera() {
	shared_ptr<Camera> cam = mySM->createCamera("MyCamera", 1, 1000, XM_PI / 4.0);
	shared_ptr<SceneNode> cn = mySM->createSceneNode("CamNode");
	shared_ptr<SceneNode> cn2 = mySM->createSceneNode("CamNode2");
	cn->setPosition(XMVectorSet(0, 0, 0, 1));
	mySM->getRootSceneNode()->addChild(cn);
	cn->addChild(cn2);
	cn2->setPosition(XMVectorSet(0, 0, 0, 1));
	cn2->attachObject(cam);


	shared_ptr<Material> laserMaterial = assetM->createMaterial("Red");
	laserMaterial->setTexture(std::wstring(L"..\\..\\Textures\\red.bmp"));
	shared_ptr<Material> green = assetM->createMaterial("Green");
	green->setTexture(std::wstring(L"..\\..\\Textures\\green.bmp"));

	shared_ptr<MeshCube> laserMash = assetM->createMeshCube("MeshCube");

	laser = new Laser("Laser", laserMaterial,green, laserMash);


	
}

void myFrameListener::addGameObject() {

	shared_ptr<Entity> sun = mySM->createEntity("Sun");
	shared_ptr<SceneNode> sunnode = mySM->createSceneNode("SunNode");
	mySM->getRootSceneNode()->addChild(sunnode);
	sunnode->attachObject(sun);

	sunnode->setPosition(XMVectorSet(70, 70, 70, 1));
	sunnode->setScaling(XMVectorSet(50, 50, 50, 1));

	shared_ptr<Material> matsun = assetM->createMaterial("Sun");
	matsun->setTexture(std::wstring(L"..\\..\\Textures\\sunTexture.dds"));

	sun->setMaterial(matsun);

	shared_ptr<MeshSphere> sunmesh = assetM->createMeshSphere("MeshSun", 32, 32);
	sun->setMesh(sunmesh);


	addAsteroid();

}

void myFrameListener::addAsteroid() {

	shared_ptr<Material> matasteroid = assetM->createMaterial("Asteroid");
	matasteroid->setTexture(std::wstring(L"..\\..\\Textures\\moonTexture.dds"));


	shared_ptr<Material> blue = assetM->createMaterial("Blue");
	blue->setTexture(std::wstring(L"..\\..\\Textures\\blue.bmp"));

	shared_ptr<MeshSphere> meshasteroid = assetM->createMeshSphere("MeshAsteroid", 16, 16);
	shared_ptr<SceneNode> nodeAsteroids = mySM->createSceneNode("MoonNode");

	nodeAsteroids->setPosition(XMVectorSet(70, 70, 70, 1.0f));
	for (int i = 0; i < NUM_DEBRIS; i++) {
		isRemoved[i] = false;
		string name = "asteroid" + i;
		debris[i] = new Asteroid(name, matasteroid, meshasteroid);
		debris[i]->setPos(XMVectorSet((rand() % 199) - 100, (rand() % 99) - 50, (rand() % 199) - 100, 1));
		nodeAsteroids->addChild(debris[i]->getNode());


		/*
		shared_ptr<Entity> entity = mySM->createEntity("entity" + name);
		shared_ptr<SceneNode> node = mySM->createSceneNode("node" + name);
		nodeAsteroids->addChild(node);
		node->attachObject(entity);

		entity->setMaterial(blue);
		entity->setMesh(meshasteroid);
		int xp = (rand() % 499)-250, yp =(rand() % 199)-100, zp=  (rand() % 499)-250;

		if (yp > 50 && yp < -50) {
		if (yp > 50)
		yp += 50;
		else
		yp -= 50;
		}
		node->setPosition(XMVectorSet(xp,yp,zp, 1.0f));
		*/
	}
	mySM->getRootSceneNode()->addChild(nodeAsteroids);
}

void myFrameListener::frameStarted(float dt)
{
	static float time = 0;

	time += dt;


	shared_ptr<SceneNode> sn = mySM->getSceneNode("CamNode");
	shared_ptr<SceneNode> sn2 = mySM->getSceneNode("CamNode2");

	dt *= 5.0f;

	shared_ptr<SceneNode> a = mySM->getSceneNode("SunNode");
	shared_ptr<SceneNode> b = mySM->getSceneNode("MoonNode");
	a->yaw(XM_PI / 36.0f*dt);
	b->yaw(-XM_PI / 256.0f*dt);
	b->roll(XM_PI / 128.0f*dt);
	if (laser->isFiring())
		laser->laserOff();

	bool keyLPressed = false;
	for (UINT i = 0; i<mKeysPressed.size() && sn; i++) {
		switch (mKeysPressed[i]) {
		case VK_LEFT:
			sn->rotateAxis(XMVectorSet(0, 1, 0, 0), -XM_PI / 12.0f*dt, SceneNode::TS_WORLD);
			break;
		case VK_RIGHT:
			sn->rotateAxis(XMVectorSet(0, 1, 0, 0), XM_PI / 12.0f*dt, SceneNode::TS_WORLD);
			break;
		case VK_UP:
			sn2->pitch(-XM_PI / 12.0f*dt);
			break;
		case VK_DOWN:
			sn2->pitch(XM_PI / 12.0f*dt);
			break;

		case 'A':
			sn->translate(dt*XMVectorSet(-5, 0, 0, 0), SceneNode::TS_LOCAL);
			break;
		case 'D':
			sn->translate(dt*XMVectorSet(5, 0, 0, 0), SceneNode::TS_LOCAL);
			break;

		case 'W':
			sn->translate(dt * 5 * XMVector3Normalize(sn2->getDirectionZ(SceneNode::TS_WORLD)), SceneNode::TS_WORLD);
			break;
		case 'S':
			sn->translate(-dt * 5 * XMVector3Normalize(sn2->getDirectionZ(SceneNode::TS_WORLD)), SceneNode::TS_WORLD);
			break;

		case 'E':
			sn2->roll(-XM_PI / 12.0f*dt);
			break;
		case 'Q':
			sn2->roll(XM_PI / 12.0f*dt);
			break;
		case VK_SPACE:
			laser->laserOn();
		break;
		}


	}

	if(laser->isFiring())
	handleLaserCollisions();

	if (handleShipCollisions()) {
		damageFrame = 29;
	}

	if (damageFrame % 8 == 5) {
		shared_ptr<Entity> sky = mySM->getEntity("Sky");
		sky->setMaterial(matsky2);
	}
	if(damageFrame % 8 == 1){
		damageFrame--;
		shared_ptr<Entity> sky = mySM->getEntity("Sky");
		sky->setMaterial(matsky);
	}

	if (damageFrame > 0) {
		damageFrame--;
	}


	win = Window::getInstance();

	std::wostringstream outs;
	outs.precision(6);
	outs << win->getWindowCaption()
		<< L"    " << L"Life: " << life
		<< L"    " << L"Asteroid: " << countAsteroid
		<< L"    " << L"Score: " << score;

	SetWindowText(win->getMainWnd(), outs.str().c_str());
	dX = 0;
	dY = 0;

	if (life == 0) {
		exit(0);
	}
	return;
}
bool myFrameListener::handleLaserCollisions() {
	shared_ptr<SceneNode> c1 = mySM->getSceneNode("CamNode");
	shared_ptr<SceneNode> c2 = mySM->getSceneNode("CamNode2");


	XMVECTOR rayDir = XMVector3Normalize(XMVector4Transform(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), c2->getOrientation(Render::SceneNode::TransformSpace::TS_WORLD)));
	XMVECTOR camPos = c1->getPosition(Render::SceneNode::TransformSpace::TS_WORLD);

	for (int i = 0; i < NUM_DEBRIS; i++)
	{
		if (!debris[i]->isDestroyed())
		{

			XMVECTOR center = debris[i]->getNode()->getPosition(Render::SceneNode::TransformSpace::TS_WORLD);
			XMVECTOR diff = XMVectorSubtract(camPos, center);

			float rayDirDotDiff = (XMVector3Dot(rayDir, diff)).m128_f32[0];
			float magDiff = (XMVector3Length(diff)).m128_f32[0];

			float radius = 1.0f;
			float discriminant = rayDirDotDiff * rayDirDotDiff - magDiff * magDiff + radius * radius;
			if (discriminant >= 0)
			{
				debris[i]->destroy();
				countAsteroid--;
				score += 100;
				return true;

			}

		}
	}
	return false;
}
bool myFrameListener::handleShipCollisions() {
	shared_ptr<SceneNode> cn = mySM->getSceneNode("CamNode");
	float playerX = cn->mPosition.x;
	float playerY = cn->mPosition.y;
	float playerZ = cn->mPosition.z;


	for (int i = 0; i < NUM_DEBRIS; i++) {
		if (!debris[i]->isDestroyed()) {
			string name = "asteroid" + i;
			shared_ptr<SceneNode> b = mySM->getSceneNode("node" + name);

			float bX = XMVectorGetX(b->getPosition(Render::SceneNode::TransformSpace::TS_WORLD));
			float bY = XMVectorGetY(b->getPosition(Render::SceneNode::TransformSpace::TS_WORLD));
			float bZ = XMVectorGetZ(b->getPosition(Render::SceneNode::TransformSpace::TS_WORLD));

			float distsq = (playerX - bX)*(playerX - bX) + (playerY - bY)*(playerY - bY) + (playerZ - bZ)*(playerZ - bZ);
			if (distsq < 1.0f) {
				debris[i]->destroy();
				life--;
				countAsteroid--;
				return true;
			}
		}

	}

	return false;
}



LRESULT myFrameListener::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (!FrameListener::MsgProc(hwnd, msg, wParam, lParam))
		return 0;

	return 1;
}


// Convenience overrides for handling mouse input.
void myFrameListener::OnMouseDown(WPARAM btnState, int x, int y)
{
	mMouseDown = true;
}

void myFrameListener::OnMouseUp(WPARAM btnState, int x, int y)
{
	mMouseDown = false;
}


void myFrameListener::OnMouseMove(WPARAM btnState, int x, int y)
{
	dX += x - mX;
	dY += y - mY;

	mX = x;
	mY = y;
}


