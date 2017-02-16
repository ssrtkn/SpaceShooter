#pragma once

#include "D3DUtility.h"
#include "gameobject.h"
#include "SceneNode.h"
#include "SceneManager.h"

namespace Render {

class Animation : public GameObject
{
	friend SceneManager;

	enum AnimationType {
		ANIM_ONCE,
		ANIM_PERIODIC
	};

	struct KeyNode {
		float    time;
		XMFLOAT4 position;
		XMFLOAT4 orientation;
		XMFLOAT3 scale;
	};

	struct AnimationSequence {
		shared_ptr<SceneNode> sceneNode;
		KeyNode initNode;
		std::vector<KeyNode> keyNodes;
		UINT currentNode;
	};

public:
	RENDERDLL_API Animation(std::string Name);
	RENDERDLL_API ~Animation(void);
	RENDERDLL_API void setAnimationType( AnimationType type ){ mAnimType = type; };
	RENDERDLL_API AnimationType getAnimationType(){ return mAnimType; };
	RENDERDLL_API void addSequence( AnimationSequence sequence ){ mSequences.push_back( sequence ); };

protected:
	void startAnimation();
	void tickAnimation( float deltatime );
	bool tickSequence(AnimationSequence& seq, float newTime );

protected:
	AnimationType mAnimType;
	bool mPlaying;
	float mAnimationTime;
	std::vector<AnimationSequence> mSequences;
};

}