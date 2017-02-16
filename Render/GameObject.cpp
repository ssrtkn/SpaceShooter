
#include <sstream>
#include "GameObject.h"

using namespace Render;

unsigned int GameObject::autonumber = 0;

//constructor
GameObject::GameObject(std::string Name)
{
	autonumber++;	//increase object counter

	if( Name.length() == 0 ) {			//if no name is given then use default name
		std::stringstream out;
		out << "Object" << autonumber;
		mName = out.str();
		return;
	}
    mName = Name.c_str();
}

//destructor
GameObject::~GameObject()
{
}

//return the object name
std::string GameObject::getName()
{
    return mName;
}
