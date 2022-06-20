#pragma once
#include "Component.h"

class RigidbodyComponent : public Component
{
public:
	RigidbodyComponent() = delete;
	RigidbodyComponent(Object* pObject);
	~RigidbodyComponent();

public:
	virtual void SolveConstraint();
	virtual void Update(float fTimeElapsed);

protected:
	XMFLOAT3 m_xmf3PrevPosition;
};

