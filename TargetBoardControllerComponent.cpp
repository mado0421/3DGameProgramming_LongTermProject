#include "stdafx.h"
#include "TargetBoardControllerComponent.h"

TargetBoardControllerComponent::TargetBoardControllerComponent(Object* pObject)
	: Component(pObject)
	, Character(50)
{
}

TargetBoardControllerComponent::~TargetBoardControllerComponent()
{
}

void TargetBoardControllerComponent::Update(float fTimeElapsed)
{
	if (!m_bEnabled) return;

	Character::Update(fTimeElapsed);
}
