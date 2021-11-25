#include "stdafx.h"
#include "Components.h"
#include "Object.h"

ControllerComponent::ControllerComponent(Object* pObject)
	:Component(pObject, "Controller")
{
}

ControllerComponent::~ControllerComponent()
{
}

void ControllerComponent::InputEvent(UCHAR* pKeyBuffer)
{
	m_xmf3Direction = XMFLOAT3(0, 0, 0);
	if (pKeyBuffer[KeyCode::_W] & 0xF0) { m_xmf3Direction.z += m_fSpeed; }
	if (pKeyBuffer[KeyCode::_A] & 0xF0) { m_xmf3Direction.x -= m_fSpeed; }
	if (pKeyBuffer[KeyCode::_S] & 0xF0) { m_xmf3Direction.z -= m_fSpeed; }
	if (pKeyBuffer[KeyCode::_D] & 0xF0) { m_xmf3Direction.x += m_fSpeed; }
	if (pKeyBuffer[KeyCode::_R] & 0xF0) { m_xmf3Direction.y += m_fSpeed; }
	if (pKeyBuffer[KeyCode::_F] & 0xF0) { m_xmf3Direction.y -= m_fSpeed; }
}

void ControllerComponent::Update(float fTimeElapsed)
{
	if (!m_bEnabled) return;

	dynamic_cast<TransformComponent*>(m_pObject->FindComponentByName("Transform"))
		->Translate(Vector3::Multiply(fTimeElapsed, m_xmf3Direction));
}
