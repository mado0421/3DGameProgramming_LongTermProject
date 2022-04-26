#pragma once
#include "Component.h"

class GameManagerComponent : public Component
{
public:
	GameManagerComponent() = delete;
	GameManagerComponent(Object* pObject);
	~GameManagerComponent();

	virtual void Update(float fTimeElapsed);
private:
};

