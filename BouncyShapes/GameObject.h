#pragma once

#include <memory>
#include "GeometryPrimitives.h"
#include "IRenderer.h"



class GameObject {
public:
	GameObject(std::shared_ptr<IRenderer> renderer, MPoint2F location, Speed2DF speed);

	const IRenderer& getRenderer();
	const MPoint2F& getLocation();
	const Speed2DF& getSpeed();
	void setSpeed(float& vel);
	void setLocation(MPoint2F newLocation);

private:
	std::shared_ptr<IRenderer> _rendererPtr;
	MPoint2F _location;
	Speed2DF _speed;
};