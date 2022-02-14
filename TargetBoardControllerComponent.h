#pragma once
#include "Component.h"
#include "Character.h"


class TargetBoardControllerComponent : public Component, public Character
{
public:
	TargetBoardControllerComponent() = delete;
	TargetBoardControllerComponent(Object* pObject);
	~TargetBoardControllerComponent();

public:
	virtual void Update(float fTimeElapsed);
};

