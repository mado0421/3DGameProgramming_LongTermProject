#include "stdafx.h"
#include "TargetBoardControllerComponent.h"

TargetBoardControllerComponent::TargetBoardControllerComponent(Object* pObject, bool bAutoRevive)
	: Component(pObject)
	, Character(10, bAutoRevive)
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
