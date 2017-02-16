

#include "Root.h"
#include "myFrameListener.h"
#include "EntityLight.h"
#include "EntitySky.h"
#include "EntityTerrain.h"
#include "EntityBillboard.h"
#include "EntityColor.h"
#include "SceneManager.h"
#include "AssetManager.h"
#include "SceneNode.h"
#include "Mesh.h"
#include "MeshCube.h"
#include "MeshGrid.h"
#include "MeshSphere.h"
#include "MeshFromFile.h"

using namespace Render;

shared_ptr<SceneNode> createSceneNodeFromFile(std::string name, std::wstring filename, std::wstring texturepath)
{
	SceneManager* scnmgr = SceneManager::getInstance();
	AssetManager* astmgr = AssetManager::getInstance();

	std::string _filename(filename.begin(), filename.end());
	UINT nummeshes = astmgr->getNumMeshes(_filename);
	if(!nummeshes)
		return 0;
	UINT nummaterials = astmgr->getNumMaterials(_filename);

	shared_ptr<SceneNode> node = scnmgr->createSceneNode(std::string("Node") + name);

	char buf[16];
	std::string strsubset;
	for(UINT s = 0; s < nummeshes; s++)
	{
		_itoa_s(s + 1, buf, 10);
		strsubset = std::string("_Subset") + std::string(buf);

		shared_ptr<Entity> entityhuman_s1 = scnmgr->createEntity(std::string("Node") + name + strsubset);
		if(s < nummaterials)
			entityhuman_s1->setMaterial( astmgr->createMaterialFromFile(std::string("Mat") + name + strsubset, filename, texturepath, s) );
		entityhuman_s1->setMesh( astmgr->createMeshFromFile(std::string("Mesh") + name + strsubset, filename, s) );
		node->attachObject( entityhuman_s1 );
	}

	return node;
}

//The program starts HERE
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	Root *myRoot = Root::getInstance();
	SceneManager *mySM = SceneManager::getInstance();
	AssetManager *assetM = AssetManager::getInstance();

	myFrameListener myListener("FL1");
	myRoot->addFrameListener( &myListener );

	shared_ptr<Light> dirlight = mySM->createLight("DirectionalLight");
	dirlight->setDirectional(	XMFLOAT4(0.4f,0.4f,0.4f,0.0f), 
								XMFLOAT4(0.8f,0.8f,0.8f,0.0f), 
								XMFLOAT4(0.2f,0.2f,0.2f,0.0f));
	shared_ptr<SceneNode> ln = mySM->createSceneNode("DirLightNode");
	mySM->getRootSceneNode()->addChild(ln);
	ln->attachObject( dirlight );
	ln->setPosition( XMVectorSet(200,200,0,1) );
	//ln->pitch( XM_PI );
	ln->lookAt( XMVectorSet( 0,0,0,1) );

	/*Light * pointlight = mySM->createLight("PointLight");
	pointlight->setPointLight(	XMFLOAT4(0.1,0.1,0.1,0), XMFLOAT4(0.1,0.1,0.1,0), XMFLOAT4(0.6,0.6,0.6,10), 
								30, XMFLOAT3(1,0,0) ); 
	SceneNode *pln = mySM->createSceneNode("PointLightNode");
	mySM->getRootSceneNode()->addChild(pln);
	pln->attachObject( pointlight );
	pln->setPosition( XMVectorSet( 0,5,5,1) );
	pln->lookAt( XMVectorSet( 0,0,0,1) );

	shared_ptr<Light> spotlight = mySM->createLight("SpotLight");
	spotlight->setSpotLight(	XMFLOAT4(0.1,0.1,0.1,0), 
								XMFLOAT4(0.8,0,0,0), 
								XMFLOAT4(0.1,0,0,1), 
								30, 3, XMFLOAT3(1,0,0) ); 
	shared_ptr<SceneNode> sln = mySM->createSceneNode("SpotLightNode");
	mySM->getRootSceneNode()->addChild(sln);
	sln->attachObject( spotlight );
	sln->setPosition( XMVectorSet( 0,60,2,1) );
	sln->lookAt( XMVectorSet( 0,0,0,1) );*/

	//------------------------------------------------

	shared_ptr<Camera> cam = mySM->createCamera("MyCamera", 1, 1000, XM_PI/4.0);
	shared_ptr<SceneNode> cn = mySM->createSceneNode("CamNode");
	shared_ptr<SceneNode> cn2 = mySM->createSceneNode("CamNode2");
	cn->setPosition( XMVectorSet( 0,80,-50,1) );
	mySM->getRootSceneNode()->addChild(cn);
	cn->addChild( cn2 );
	cn2->setPosition( XMVectorSet( 0,0,0,1) );
	cn2->attachObject( cam );

	//------------------------------------------------

	shared_ptr<Entity> sky = mySM->createEntity("Sky", Entity::ET_SKY);
	shared_ptr<SceneNode> skynode = mySM->createSceneNode("SkyNode");
	mySM->getRootSceneNode()->addChild(skynode);
	skynode->attachObject( sky );
	
	shared_ptr<Material> matsky = assetM->createMaterial("Sky");
	matsky->setTexture(std::wstring(L"Textures\\grasscube1024.dds"));
	sky->setMaterial( matsky );

	shared_ptr<MeshSphere> skymesh = assetM->createMeshSphere( "MeshSky", 10, 10 );
	sky->setMesh( skymesh );

	//------------------------------------------------

	shared_ptr<Entity> grid = mySM->createEntity("Grid", Entity::ET_LIGHTNORMAL);
	shared_ptr<SceneNode> gridnode = mySM->createSceneNode("GridNode");
	gridnode->setPosition( XMVectorSet(-200,60, 0,1) );
	gridnode->setScaling( XMVectorSet( 400,1,300,0 ));
	mySM->getRootSceneNode()->addChild( gridnode );
	gridnode->attachObject( grid );

	shared_ptr<Material> matbricks = assetM->createMaterial("Bricks");
	matbricks->setTexture(std::wstring(L"Textures\\bricks.dds"));
	matbricks->setNormalMap(std::wstring(L"Textures\\bricks_nmap.dds"));
	grid->setMaterial( matbricks );

	shared_ptr<Mesh> gridmesh = assetM->createMeshGrid( "MeshGrid", 3, 3, 10 );
	grid->setMesh( gridmesh );
	
	//------------------------------------------------

	/*shared_ptr<Entity> terrain = mySM->createEntity("Terrain", Entity::ET_TERRAIN);
	shared_ptr<SceneNode> terrainnode = mySM->createSceneNode("TerrainNode");
	terrainnode->setPosition( XMVectorSet(0,0,0,1) );
	terrainnode->setScaling( XMVectorSet( 2000,50,2000,0 ));
	mySM->getRootSceneNode()->addChild( terrainnode );
	terrainnode->attachObject( terrain );

	shared_ptr<Material> matterrain = assetM->createMaterial("Terrain");
	std::vector<std::wstring> layerFilenames;
	layerFilenames.push_back(std::wstring(L"Textures/grass.dds"));
	layerFilenames.push_back(std::wstring(L"Textures/darkdirt.dds"));
	layerFilenames.push_back(std::wstring(L"Textures/stone.dds"));
	layerFilenames.push_back(std::wstring(L"Textures/lightdirt.dds"));
	layerFilenames.push_back(std::wstring(L"Textures/snow.dds"));
	matterrain->setTextures( layerFilenames);
	matterrain->setBlendMap( std::wstring(L"Textures/blend.dds" ));
	terrain->setMaterial( matterrain );

	shared_ptr<MeshGrid> terrainmesh = assetM->createMeshGrid("MeshTerrain",200, 200, 100);
	terrainmesh->setHeightMap(std::wstring(L"Textures\\terrain.raw"));
	terrain->setMesh( terrainmesh );

	//------------------------------------------------

	shared_ptr<Entity> sphere = mySM->createEntity("Sphere");
	shared_ptr<SceneNode> spherenode = mySM->createSceneNode("SphereNode");
	spherenode->setPosition( XMVectorSet(10,80,10,1) );
	spherenode->setScaling( XMVectorSet( 50,20,50,0 ));
	mySM->getRootSceneNode()->addChild( spherenode );
	spherenode->attachObject( sphere );

	shared_ptr<Material> matwater = assetM->createMaterial("Water");
	matwater->setTexture(std::wstring(L"Textures\\water2.dds"));
	sphere->setMaterial( matwater );

	shared_ptr<MeshSphere> spheremesh = assetM->createMeshSphere("MeshSphere", 30,30, 15);
	sphere->setMesh( spheremesh );
	
	//------------------------------------------------

	shared_ptr<Entity> cube2 = mySM->createEntity("Cube2");
	shared_ptr<SceneNode> node2 = mySM->createSceneNode("Node2");
	node2->setPosition( XMVectorSet(100,150,100,1) );
	mySM->getRootSceneNode()->addChild( node2 );
	node2->attachObject( cube2 );
	node2->setScaling( XMVectorSet( 20.0f,20.0f,20.0f, 0.0f) );

	shared_ptr<Material> material2 = assetM->createMaterial("Wood2");
	material2->setTexture( std::wstring(L"Textures\\WoodCrate01.dds"));
	cube2->setMaterial( material2 );

	shared_ptr<MeshCube> mesh = assetM->createMeshCube("MeshCube");
	cube2->setMesh( mesh );

	//------------------------------------------------

	shared_ptr<Entity> cube3 = mySM->createEntity("Cube3");
	shared_ptr<SceneNode> node3 = mySM->createSceneNode("Node3");
	node3->setPosition( XMVectorSet(0,150,5,1) );
	mySM->getRootSceneNode()->addChild( node3 );
	node3->attachObject( cube3 );
	node3->setScaling( XMVectorSet( 10.0f,30.0f,10.0f, 0.0f) );

	shared_ptr<Material> material3 = assetM->createMaterial("Wood3");
	material3->setTexture( std::wstring(L"Textures\\WoodCrate02.dds"));
	cube3->setMaterial( material3 );
	cube3->setMesh( mesh );

	node2->yaw( -XM_PI/2.0f) ;
	//node3->pitch( -XM_PI/2.0f) ;
	//node3->translate( XMVectorSet( 0,0,5,0 ), SceneNode::TS_PARENT );
	
	//------------------------------------------------

	// moving sphere

	shared_ptr<Entity> movsphere = mySM->createEntity("MovingSphere");
	shared_ptr<SceneNode> movspherenode = mySM->createSceneNode("MovingSphereNode");

	shared_ptr<EntityTerrain> et = dynamic_pointer_cast<EntityTerrain>(terrain);
	movspherenode->setPosition( et->projectOnTerrain( XMVectorSet(0,0,-400,1) ) );
	movspherenode->setScaling( XMVectorSet( 5,5,5,0 ));
	mySM->getRootSceneNode()->addChild( movspherenode );
	movspherenode->attachObject( movsphere );

	shared_ptr<Material> matstone = assetM->createMaterial("Stone");
	matstone->setTexture(std::wstring(L"Textures\\stone.dds"));
	movsphere->setMaterial( matstone );

	shared_ptr<Mesh> movspheremesh = assetM->createMeshSphere("MeshMovingSphere", 30,30, 15);
	movsphere->setMesh( movspheremesh );


	//------------------------------------------------
	shared_ptr<Entity> billboard0 = mySM->createEntity("Billboard0", Entity::ET_BILLBOARD );
	terrainnode->attachObject( billboard0 );

	shared_ptr<Mesh> bill0mesh = assetM->createMesh("MeshBillboard0" );
	billboard0->setMesh( bill0mesh );
	std::default_random_engine generator;
	std::uniform_real_distribution<float> distribution(-1,1);
	for( UINT i=0; i<100; i++ ) {
		Mesh::Vertex v;
		v.Pos = XMFLOAT3( distribution(generator), 0, distribution(generator) );
		v.Pos.y = terrainmesh->getY( v.Pos.x, v.Pos.z );
		v.Size = XMFLOAT2( 20, 40 );
		bill0mesh->addVertex( v );
	}
	shared_ptr<Material> mattree0 = assetM->createMaterial("Tree0");
	mattree0->setTexture( std::wstring(L"Textures\\Tree0.dds") );
	billboard0->setMaterial( mattree0 );

	//------------------------------------------------
	shared_ptr<Entity> billboard1 = mySM->createEntity("Billboard1", Entity::ET_BILLBOARD );
	terrainnode->attachObject( billboard1 );
	shared_ptr<Mesh> bill1mesh = assetM->createMesh("MeshBillboard1" );
	billboard1->setMesh( bill1mesh );
	for( UINT i=0; i<100; i++ ) {
		Mesh::Vertex v;
		v.Pos = XMFLOAT3( distribution(generator), 0, distribution(generator) );
		v.Pos.y = terrainmesh->getY( v.Pos.x, v.Pos.z );
		v.Size = XMFLOAT2( 20, 40 );
		bill1mesh->addVertex( v );
	}
	shared_ptr<Material> mattree1 = assetM->createMaterial("Tree1");
	mattree1->setTexture( std::wstring(L"Textures\\Tree1.dds"));
	billboard1->setMaterial( mattree1 );

	//------------------------------------------------
	shared_ptr<Entity> billboard2 = mySM->createEntity("Billboard2", Entity::ET_BILLBOARD );
	terrainnode->attachObject( billboard2 );
	shared_ptr<Mesh> bill2mesh = assetM->createMesh("MeshBillboard2" );
	billboard2->setMesh( bill2mesh );
	for( UINT i=0; i<100; i++ ) {
		Mesh::Vertex v;
		v.Pos = XMFLOAT3( distribution(generator), 0, distribution(generator) );
		v.Pos.y = terrainmesh->getY( v.Pos.x, v.Pos.z );
		v.Size = XMFLOAT2( 20, 40 );
		bill2mesh->addVertex( v );
	}
	shared_ptr<Material> mattree2 = assetM->createMaterial("Tree2");
	mattree2->setTexture(std::wstring(L"Textures\\Tree3.dds") );
	billboard2->setMaterial( mattree2 );

	//------------------------------------------------
	shared_ptr<EntityLight> billboard3 = dynamic_pointer_cast<EntityLight>(mySM->createEntity("BillboardShadowMap" ));
	shared_ptr<SceneNode> shadownode = mySM->createSceneNode("BillboardShadowNode");
	shadownode->attachObject( billboard3 );
	cn2->addChild( shadownode );
	shadownode->pitch( -XM_PI/2.0f);
	shadownode->yaw( XM_PI );
	shadownode->translate( XMVectorSet(-1.3f, -1.0f, 6.0f, 0.0f), SceneNode::TS_PARENT );
	billboard3->disableLighting();

	shared_ptr<MeshGrid> billmeshshadow = assetM->createMeshGrid("MeshBillboardShadow", 3,3,1 );
	billboard3->setMesh( billmeshshadow );

	shared_ptr<Material> matbillshadow = assetM->createMaterial("MatBillShadow");
	matbillshadow->setTexture( mySM->getShadow()->getDepthMapSRV() );
	billboard3->setMaterial( matbillshadow );*/
	
	//------------------------------------------------

	/*shared_ptr<EntityColor> frustumcube = dynamic_pointer_cast<EntityColor>(mySM->createEntity("ShadowCamFrustumCube" , Entity::ET_COLOR ) );
	frustumcube->setCastShadow( false );
	shared_ptr<SceneNode> frustumnode = mySM->createSceneNode("ShadowCamFrustumNode");
	frustumnode->attachObject( frustumcube );
	mySM->getRootSceneNode()->addChild( frustumnode );

	shared_ptr<MeshCube> meshfc = assetM->createMeshCube("ShadowCamFrustumMesh");
	frustumcube->setMesh( meshfc );
	shared_ptr<Material> matfc = assetM->createMaterial("ShadowCamFrustumMaterial");
	matfc->setMaterial( XMFLOAT4(0,0,0,0), XMFLOAT4(0.8,0.3,0.3,0), XMFLOAT4(0,0,0,0), XMFLOAT4(0,0,0,0) );
	frustumcube->setMaterial( matfc );*/

	//------------------------------------------------


	//bird cam

	/*Window *win = Window::getInstance();
	shared_ptr<Viewport> vp = win->createViewPort("BirdVP", 0.7,0.7,0.25,0.25 );
	shared_ptr<Camera> bcam = mySM->createCamera("MyBirdCamera", vp);
	shared_ptr<SceneNode> bcn = mySM->createSceneNode("BirdCamNode");
	bcn->setPosition( XMVectorSet( 0,0,0,1 ) );
	bcn->translate( XMVectorSet(0,100,0,0), SceneNode::TS_WORLD ); 
	cn->addChild(bcn);
	bcn->pitch( XM_PI/2.0f );
	bcn->attachObject( bcam );

	//back mirror cam
	shared_ptr<Viewport> mvp = win->createViewPort("MirrorVP", 0.7,0.1,0.2,0.2 );
	shared_ptr<Camera> mcam = mySM->createCamera("MyMirrorCamera", mvp);
	shared_ptr<SceneNode> mcn = mySM->createSceneNode("MirrorCamNode");
	mcn->setPosition( XMVectorSet( 0,0,0,1 ) );
	mcn->translate( XMVectorSet(0,2,0,0), SceneNode::TS_WORLD ); 
	cn2->addChild(mcn);
	mcn->yaw( XM_PI );
	mcn->attachObject( mcam );
	*/


	//------------------------------------------------

	// teapot from file

	/*shared_ptr<Entity> entityteapot = mySM->createEntity("EntityTeapot");
	shared_ptr<SceneNode> nodeteapot = mySM->createSceneNode("NodeTeapot");
	nodeteapot->setPosition(XMVectorSet(30.0f, 100.0f, 100.0f, 1.0f));
	nodeteapot->setScaling(XMVectorSet(20.0f, 20.0f, 20.0f, 0.0f));
	mySM->getRootSceneNode()->addChild( nodeteapot );
	nodeteapot->attachObject( entityteapot );

	shared_ptr<Material> matteapot = assetM->createMaterial("MatTeapot");
	matteapot->setTexture(std::wstring(L"Textures\\brick01.dds"));
	entityteapot->setMaterial( matteapot );

	shared_ptr<Mesh> meshteapot = assetM->createMeshFromFile("MeshTeapot", L"Entities\\teapot.x");
	entityteapot->setMesh( meshteapot );

	//------------------------------------------------

	// cube from file

	shared_ptr<Entity> entitycube = mySM->createEntity("EntityCube");
	shared_ptr<SceneNode> nodecube = mySM->createSceneNode("NodeCube");
	nodecube->setPosition(XMVectorSet(-30.0f, 100.0f, 100.0f, 1.0f));
	nodecube->setScaling(XMVectorSet(20.0f, 20.0f, 20.0f, 0.0f));
	mySM->getRootSceneNode()->addChild( nodecube );
	nodecube->attachObject( entitycube );

	shared_ptr<Material> matcube = assetM->createMaterial("MatCube");
	matcube->setTexture(std::wstring(L"Textures\\brick01.dds"));
	entitycube->setMaterial( matcube );

	shared_ptr<Mesh> meshcube = assetM->createMeshFromFile("MeshCube", L"Entities\\cube.x");
	entitycube->setMesh( meshcube );*/

	//------------------------------------------------

	// human from file

#define USE_CREATESCENENODEFROMFILE

#ifdef USE_CREATESCENENODEFROMFILE
	shared_ptr<SceneNode> nodehuman = createSceneNodeFromFile("Human", L"Entities\\human.x", L"Textures\\");
#else
	shared_ptr<SceneNode> nodehuman = mySM->createSceneNode("NodeHuman");

	shared_ptr<Entity> entityhuman_s1 = mySM->createEntity("EntityHuman_Subset1");
	entityhuman_s1->setMaterial( assetM->createMaterialFromFile("MatHuman_Subset1", L"Entities\\human.x", L"Textures\\", 0) );
	entityhuman_s1->setMesh( assetM->createMeshFromFile("MeshHuman_Subset1", L"Entities\\human.x", 0) );
	nodehuman->attachObject( entityhuman_s1 );

	shared_ptr<Entity> entityhuman_s2 = mySM->createEntity("EntityHuman_Subset2");
	entityhuman_s2->setMaterial( assetM->createMaterialFromFile("MatHuman_Subset2", L"Entities\\human.x", L"Textures\\", 1) );
	entityhuman_s2->setMesh( assetM->createMeshFromFile("MeshHuman_Subset2", L"Entities\\human.x", 1) );
	nodehuman->attachObject( entityhuman_s2 );

	shared_ptr<Entity> entityhuman_s3 = mySM->createEntity("EntityHuman_Subset3");
	entityhuman_s3->setMaterial( assetM->createMaterialFromFile("MatHuman_Subset3", L"Entities\\human.x", L"Textures\\", 2) );
	entityhuman_s3->setMesh( assetM->createMeshFromFile("MeshHuman_Subset3", L"Entities\\human.x", 2) );
	nodehuman->attachObject( entityhuman_s3 );

	shared_ptr<Entity> entityhuman_s4 = mySM->createEntity("EntityHuman_Subset4");
	entityhuman_s4->setMaterial( assetM->createMaterialFromFile("MatHuman_Subset4", L"Entities\\human.x", L"Textures\\", 3) );
	entityhuman_s4->setMesh( assetM->createMeshFromFile("MeshHuman_Subset4", L"Entities\\human.x", 3) );
	nodehuman->attachObject( entityhuman_s4 );

	shared_ptr<Entity> entityhuman_s5 = mySM->createEntity("EntityHuman_Subset5");
	entityhuman_s5->setMaterial( assetM->createMaterialFromFile("MatHuman_Subset5", L"Entities\\human.x", L"Textures\\", 4) );
	entityhuman_s5->setMesh( assetM->createMeshFromFile("MeshHuman_Subset5", L"Entities\\human.x", 4) );
	nodehuman->attachObject( entityhuman_s5 );
#endif

	nodehuman->setPosition(XMVectorSet(-30.0f, 100.0f, 100.0f, 1.0f));
	nodehuman->setScaling(XMVectorSet(20.0f, 20.0f, 20.0f, 0.0f));
	mySM->getRootSceneNode()->addChild( nodehuman );

	//------------------------------------------------

	// weapon from file

	#ifdef USE_CREATESCENENODEFROMFILE
	shared_ptr<SceneNode> nodeweapon = createSceneNodeFromFile("Weapon", L"Entities\\desert_eagle.x", L"Textures\\");
#else
	shared_ptr<SceneNode> nodeweapon = mySM->createSceneNode("NodeWeapon");
	shared_ptr<Entity> entityweapon = mySM->createEntity("EntityWeapon");
	entityweapon->setMaterial( assetM->createMaterialFromFile("MatWeapon", L"Entities\\desert_eagle.x", L"Textures\\", 0) );
	entityweapon->setMesh( assetM->createMeshFromFile("MeshWeapon", L"Entities\\desert_eagle.x", 0) );
	nodeweapon->attachObject( entityweapon );
#endif
	nodeweapon->setPosition(XMVectorSet(-0.72f, 0.36f, 0.0f, 1.0f));
	nodeweapon->yaw(XM_PI / 2.0f);
	nodehuman->addChild( nodeweapon );

	//------------------------------------------------

	mySM->setFog( XMFLOAT4(0.9f,0.9f,0.9f,0.5f), 4800,200 );
	mySM->enableFog();

    myRoot->startRendering();

}


