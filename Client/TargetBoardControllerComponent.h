#pragma once
#include "Component.h"
#include "Character.h"


class TargetBoardControllerComponent : public Component, public Character
{
public:
	TargetBoardControllerComponent() = delete;
	TargetBoardControllerComponent(Object* pObject, bool bAutoRevive = false);
	~TargetBoardControllerComponent();

public:
	virtual void Update(float fTimeElapsed);

	void SetPlayer(Object* pO);

private:
	float m_fTime = 0.0f;
	float m_fAttackPeriod = 5.0f;

	Character* m_pPlayerCharacter = nullptr;
};

