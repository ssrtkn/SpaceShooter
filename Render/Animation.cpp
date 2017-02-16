#include "Animation.h"

using namespace Render;

Animation::Animation(std::string Name) : GameObject( Name ),mAnimationTime(0),mPlaying(false),mAnimType(ANIM_ONCE)
{
}


Animation::~Animation(void)
{
}

void Animation::startAnimation()
{
	if( !mSequences.size() )
		return;

	mPlaying = true;
	mAnimationTime = 0;

	UINT numKeyNodes=0;
	for( UINT i=0; i<mSequences.size(); i++ ) {
		mSequences[i].currentNode = -1;
		mSequences[i].initNode.time = -1.0f;
		numKeyNodes += mSequences[i].keyNodes.size();
	}
	if( numKeyNodes>0 )
		tickAnimation(0.0f);
}


void Animation::tickAnimation( float deltatime )
{
	if( !mPlaying ) return;

	mPlaying = false;
	for( UINT i=0; i<mSequences.size(); i++ ) {
		mPlaying |= tickSequence( mSequences[i], mAnimationTime + deltatime );
	}

	if( !mPlaying && mAnimType == ANIM_PERIODIC )
		startAnimation();
}


bool Animation::tickSequence(AnimationSequence& seq, float newtime )
{
	XMVECTOR position, orientation, scale;

	if( !seq.keyNodes.size() )
		return false;

	while( seq.currentNode + 1 < seq.keyNodes.size() && newtime >= seq.keyNodes[seq.currentNode+1].time ) {
		seq.currentNode++;
	}

	float t1=0.0f;
	if( seq.currentNode<0 ) {
		if( seq.initNode.time < 0.0f ) {
			seq.initNode.time = newtime;

			XMStoreFloat4( &seq.initNode.position, seq.sceneNode->getPosition() );
			XMStoreFloat4( &seq.initNode.orientation, XMQuaternionRotationMatrix( seq.sceneNode->getOrientation() ) );

			XMFLOAT4X4 smf;
			XMStoreFloat4x4( &smf, seq.sceneNode->getScaling() );
			seq.initNode.scale = XMFLOAT3( smf._11, smf._22, smf._33 );
		}
		t1			= seq.initNode.time;
		position	= XMLoadFloat4( &seq.initNode.position );
		orientation = XMLoadFloat4( &seq.initNode.orientation );
		scale		= XMLoadFloat3( &seq.initNode.scale );
	}
	else {
		t1			= seq.keyNodes[seq.currentNode].time;
		position	= XMLoadFloat4( &seq.keyNodes[seq.currentNode].position );
		orientation = XMLoadFloat4( &seq.keyNodes[seq.currentNode].orientation );
		scale		= XMLoadFloat3( &seq.keyNodes[seq.currentNode].scale );
	}

	if( seq.currentNode + 1 >= seq.keyNodes.size() ) {
		seq.sceneNode->setPosition( position );
		seq.sceneNode->setOrientation( XMMatrixRotationQuaternion( orientation ) );
		seq.sceneNode->setScaling( scale );

		if( newtime == seq.keyNodes[seq.currentNode].time )
			return true;
		return false;
	}

	float t2 = seq.keyNodes[seq.currentNode+1].time;

	if( t1 < newtime && t1 < t2 ) {
		XMVECTOR position2, orientation2, scale2;

		position2	 = XMLoadFloat4( &seq.keyNodes[seq.currentNode+1].position );
		orientation2 = XMLoadFloat4( &seq.keyNodes[seq.currentNode+1].orientation );
		scale2		 = XMLoadFloat3( &seq.keyNodes[seq.currentNode+1].scale );

		float t  = (newtime  - t1) / (t2 - t1);								//interpolate
		position    = XMVectorLerp( position, position2, t );
		orientation = XMQuaternionSlerp( orientation, orientation2, t );
		scale		= XMVectorLerp( scale, scale2, t );
	}

	seq.sceneNode->setPosition( position );
	seq.sceneNode->setOrientation( XMMatrixRotationQuaternion( orientation ) );
	seq.sceneNode->setScaling( scale );
	return true;
}


