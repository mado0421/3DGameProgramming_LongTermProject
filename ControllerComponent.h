#pragma once
#include "Component.h"

class ControllerComponent : public Component
{
public:
	ControllerComponent() = delete;
	ControllerComponent(Object* pObject);
	~ControllerComponent();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList) {}
	virtual void Update(float fTimeElapsed);
	virtual void InputEvent(UCHAR* pKeyBuffer);



public:
	float m_fSpeed = 1.0f;

protected:
	XMFLOAT3 m_xmf3Direction = XMFLOAT3(0, 0, 0);
};

