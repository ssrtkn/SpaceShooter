#pragma once
#define NUM_DEBRIS 100
#define NUM_HEALTH 25
#define NUM_ARMOR 25

#include "Render.h"
#include "Asteroid.h"
#include "Laser.h"
#include <vector>

namespace Render {

//
//My own frame listener
//
class myFrameListener : public FrameListener
{
public:
	myFrameListener( std::string name );
	~myFrameListener(void);

	virtual void frameStarted( float dt );					//called at the start of a new frame
	virtual LRESULT MsgProc(HWND hwnd, UINT msg,			//callback for windows messages 
							WPARAM wParam, LPARAM lParam); 

protected:
	int mX,mY, dX,dY;
	bool mMouseDown;

	bool mLightMove;
	bool mKeyLEnabled;
	SceneManager *mySM;
	AssetManager *assetM;
	Window *win;
	int life;
	int countAsteroid;
	int score;
	int damageFrame;
	bool isRemoved[NUM_DEBRIS];
	Asteroid *debris[NUM_DEBRIS];
	Laser *laser;
	shared_ptr<Material> matsky;
	shared_ptr<Material> matsky2;



protected:
	   // Convenience overrides for handling mouse input.
    virtual void OnMouseDown(WPARAM btnState, int x, int y);
    virtual void OnMouseUp(WPARAM btnState, int x, int y);
    virtual void OnMouseMove(WPARAM btnState, int x, int y);
	void initGame();
	void addLight();
	void addSkybox();
	void addCamera();
	void addGameObject();
	void addAsteroid();
	bool handleShipCollisions();
	bool handleLaserCollisions();
};


}


