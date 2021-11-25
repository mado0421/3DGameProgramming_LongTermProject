#pragma once
#include "Component.h"

class ControllerComponent : public Component
{
public:
	ControllerComponent() = delete;
	ControllerComponent(Object* pObject);
	~ControllerComponent();

	virtual void InputEvent(UCHAR* pKeyBuffer);
	virtual void Update(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList) {}

public:
	float m_fSpeed = 1.0f;

protected:
	XMFLOAT3 m_xmf3Direction = XMFLOAT3(0, 0, 0);
};

