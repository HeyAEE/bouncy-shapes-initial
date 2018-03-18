#pragma once
#include <vector>
#include <memory>
#include <d2d1.h>

#include "GameObject.h"

class Scene {
public:
	void insertGameObject(std::unique_ptr<GameObject>& object);

	void Update(double deltaTime);

	void Draw(ID2D1HwndRenderTarget& target);

	void BoundsCheck(D2D1_SIZE_F& size, GameObject& object);

private:
	std::vector<std::unique_ptr<GameObject>> _gameObjects = {};
};