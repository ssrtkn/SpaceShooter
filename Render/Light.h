#pragma once

#include "D3DUtility.h"
#include "MovableObject.h"

namespace Render {

class SceneManager;
class Entity;

//Light denotes standard light sources
class Light : public MovableObject
{
	friend SceneManager;
	friend Entity;

public:
	//struct for directional light parameters
	//is sent to shaders in one call
	struct DirectionalLight {
		DirectionalLight() { ZeroMemory(this, sizeof(this)); }
		XMFLOAT4 Ambient;
		XMFLOAT4 Diffuse;
		XMFLOAT4 Specular;
		XMFLOAT3 Direction;
		FLOAT Pad;
	};

	//struct for point light parameters
	//is sent to shaders in one call
	struct PointLight
	{
		PointLight() { ZeroMemory(this, sizeof(this)); }
		XMFLOAT4 Ambient;
		XMFLOAT4 Diffuse;
		XMFLOAT4 Specular;

		// Packed into 4D vector: (Position, Range)
		XMFLOAT3 Position;
		float Range;

		// Packed into 4D vector: (A0, A1, A2, Pad)
		XMFLOAT3 Att;
		float Pad; // Pad the last float so we can set an array of lights if we wanted.
	};

	//struct for spot light parameters
	//is sent to shaders in one call
	struct SpotLight
	{
		SpotLight() { ZeroMemory(this, sizeof(this)); }
		XMFLOAT4 Ambient;
		XMFLOAT4 Diffuse;
		XMFLOAT4 Specular;

		// Packed into 4D vector: (Position, Range)
		XMFLOAT3 Position;
		float Range;

		// Packed into 4D vector: (Direction, Spot)
		XMFLOAT3 Direction;
		float Spot;

		// Packed into 4D vector: (Att, Pad)
		XMFLOAT3 Att;
		float Pad; // Pad the last float so we can set an array of lights if we wanted.
	};


	enum LightTypes { LT_NONE = -1, LT_POINT = 0, LT_DIRECTIONAL = 1, LT_SPOTLIGHT = 2 };

	RENDERDLL_API ~Light(void);
	
	RENDERDLL_API LightTypes getType(){ return mType; };

	//set the light type and parameters
	RENDERDLL_API void setDirectional( XMFLOAT4 Ambient, XMFLOAT4 Diffuse, XMFLOAT4 Specular );
	RENDERDLL_API void setPointLight( XMFLOAT4 Ambient, XMFLOAT4 Diffuse, XMFLOAT4 Specular, 
						float Range,	//max light range
						XMFLOAT3 Att );	//attenuation parameters (quadratic polynomial) 
	RENDERDLL_API void setSpotLight( XMFLOAT4 Ambient, XMFLOAT4 Diffuse, XMFLOAT4 Specular,
						float Range,	//max light range
						float Spot,		//exponent for spot width
						XMFLOAT3 Att );	//attenuation parameters (quadratic polynomial) 

protected:
	Light(std::string name);	//constructor only called by SceneManager 
	LightTypes mType;			//light type

	//send the light parameters (dir/point/spot) to the shader
	void setShaderConstants(	ID3DX11EffectVariable *directional, 
								ID3DX11EffectVariable *point, 
								ID3DX11EffectVariable *spot );

	DirectionalLight mDirectionalLight;	//directional light parameters
	PointLight mPointLight;				//point light parameters
	SpotLight mSpotLight;				//spot light parameters
};

}

