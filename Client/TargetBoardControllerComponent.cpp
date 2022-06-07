#include "stdafx.h"
#include "TargetBoardControllerComponent.h"
#include "Object.h"
#include "HumanoidControllerComponent.h"

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
	if (!this->isAlive()) return;

	if (m_fTime > m_fAttackPeriod) {
		m_pPlayerCharacter->Damage(30);
		m_fTime = 0;
	}
	m_fTime += fTimeElapsed;

	Character::Update(fTimeElapsed);
}

void TargetBoardControllerComponent::SetPlayer(Object* pO)
{
	m_pPlayerCharacter = pO->FindComponent<HumanoidControllerComponent>();
}
