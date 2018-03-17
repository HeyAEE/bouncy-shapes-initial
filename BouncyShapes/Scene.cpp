#include "Scene.h"

void Scene::insertGameObject(std::unique_ptr<GameObject>& object)
{
	_gameObjects.push_back(std::move(object));
}

void Scene::Update(double deltaTime)
{
	// TODO: update objects in the scene
	/*auto object = _gameObjects.front();
	MPoint2F currentPosition = object.GetLocation();
	MPoint2F velocity{ 1.0f, 1.0f };
	MPoint2F finalPosition{ currentPosition.x + velocity.x, currentPosition.y + velocity.y };*/
	for (auto i = _gameObjects.begin(); i !=  _gameObjects.end(); ++i)
	{
		MPoint2F currentPosition = i->get()->getLocation();
		Speed2DF velocity = i->get()->getSpeed();
		MPoint2F finalPosition{ currentPosition.x + velocity.x, currentPosition.y + velocity.y };
		i->get()->setLocation(finalPosition);
	}
}

void Scene::Draw(ID2D1HwndRenderTarget & target)
{
	auto size = target.GetSize(); //Gets window size
	for (auto& object : _gameObjects)
	{
		const auto& renderer = object->getRenderer();
		renderer.Render(target, object->getLocation());
	}
}
