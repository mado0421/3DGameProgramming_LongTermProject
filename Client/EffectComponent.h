#pragma once
#include "Component.h"

class EffectComponent : public Component
{
public:
	EffectComponent() = delete;
	EffectComponent(Object* pObject);
	~EffectComponent();

public:
	virtual void Update(float fTimeElapsed);

public:
	void SetDuration(float fTime);
	void TurnOn();
	void TurnOff();

public:
	float m_fDuration;
private:
	float m_fLifetime;
};

