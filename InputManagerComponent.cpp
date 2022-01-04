#include "stdafx.h"
#include "Components.h"
#include "Object.h"

InputManagerComponent::InputManagerComponent(Object* pObject)
	:Component(pObject, "Controller")
{
}

InputManagerComponent::~InputManagerComponent()
{
}

void InputManagerComponent::InputEvent(UCHAR* pKeyBuffer)
{
	memcpy(m_pKeysBuffer, pKeyBuffer, sizeof(UCHAR) * 256);
}

void InputManagerComponent::Update(float fTimeElapsed)
{
}

bool InputManagerComponent::IsKeyDown(KeyCode key)
{
	return (m_pKeysBuffer[key] & 0xF0);
}

bool InputManagerComponent::IsKeyUp(KeyCode key)
{
	return !(m_pKeysBuffer[key] & 0xF0);
}
