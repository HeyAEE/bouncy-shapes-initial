#include "GameObject.h"

GameObject::GameObject(std::shared_ptr<IRenderer> renderer, MPoint2F location, Speed2DF speed)
	: _rendererPtr(renderer), _location(location), _speed(speed)
{
}

const IRenderer & GameObject::getRenderer()
{
	return *_rendererPtr;
}

const MPoint2F & GameObject::getLocation()
{
	return _location;
}

const Speed2DF & GameObject::getSpeed() 
{
	return _speed;
}

void GameObject::setSpeed(float& vel) 
{
	vel = -vel;
}

void GameObject::changeXSpeed()
{
	GameObject::setSpeed(GameObject::_speed.x);
}

void GameObject::changeYSpeed()
{
	GameObject::setSpeed(GameObject::_speed.y);
}

void GameObject::setLocation(MPoint2F newLocation) 
{
	_location = newLocation;
}