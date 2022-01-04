#pragma once
#include "Component.h"

class InputManagerComponent : public Component
{
public:
	InputManagerComponent() = delete;
	InputManagerComponent(Object* pObject);
	~InputManagerComponent();

	virtual void InputEvent(UCHAR* pKeyBuffer);
	virtual void Update(float fTimeElapsed);

	bool IsKeyDown(KeyCode key);
	bool IsKeyUp(KeyCode key);

protected:
	UCHAR m_pKeysBuffer[256];
};

