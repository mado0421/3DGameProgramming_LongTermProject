#pragma once
#include "Component.h"

class InputManagerComponent : public Component
{
public:
	InputManagerComponent() = delete;
	InputManagerComponent(Object* pObject);
	~InputManagerComponent();

	virtual void InputEvent(UCHAR* pKeyBuffer, XMFLOAT2& xmf2MouseMovement);
	virtual void Update(float fTimeElapsed);

	virtual bool IsKeyDown(KeyCode key);
	virtual bool IsKeyUp(KeyCode key);
	virtual const XMFLOAT2& GetMouseMovement();

protected:
	UCHAR		m_pKeysBuffer[256];
	XMFLOAT2	m_xmf2MouseMovement;

};

