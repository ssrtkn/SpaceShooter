#pragma once

#include "D3DUtility.h"
#include <string>


namespace Render {

//The root class for all main engine classes
//Just stores a string and can create an auto number
class GameObject
{
protected:
    std::string mName;

public:
    RENDERDLL_API GameObject(std::string Name);
    virtual RENDERDLL_API ~GameObject();

    RENDERDLL_API std::string getName();

protected:
	static unsigned int autonumber;
};


}