#include "stdafx.h"
#include "Object.h"
#include "NetInputManagerComponent.h"
#include "ClientWsaModule.h"

NetInputManagerComponent::NetInputManagerComponent(Object* pObject, ClientWsaModule* cwm, bool control)
	: InputManagerComponent(pObject)
{
	m_netModule = cwm;
	playerControl = control;
	cout << "NetInputManagerComponent(Object* pObject)\n";
	for (int i = 0; i < 256; ++i)m_NotBuffer[i] = (!KEYDOWN);
	player = pObject;
}

NetInputManagerComponent::~NetInputManagerComponent()
{
}

void NetInputManagerComponent::InputEvent(UCHAR* pKeyBuffer, XMFLOAT2& xmf2MouseMovement)
{
	if (true == playerControl) {
		memcpy(m_pKeysBuffer, pKeyBuffer, sizeof(UCHAR) * 256);

		//m_xmf2MouseMovement = xmf2MouseMovement;

		XMFLOAT3 l_xmf3Direction = XMFLOAT3(0, 0, 0);
		if (pKeyBuffer[KeyCode::_W] & KEYDOWN)l_xmf3Direction.z += 1.0f;
		if (pKeyBuffer[KeyCode::_A] & KEYDOWN)l_xmf3Direction.x -= 1.0f;
		if (pKeyBuffer[KeyCode::_S] & KEYDOWN)l_xmf3Direction.z -= 1.0f;
		if (pKeyBuffer[KeyCode::_D] & KEYDOWN)l_xmf3Direction.x += 1.0f;

		//if (0 != xmf2MouseMovement.x) m_xmf2MouseMovement.x = xmf2MouseMovement.x;

		if (l_xmf3Direction.x || l_xmf3Direction.z || xmf2MouseMovement.x)
		{
			// Send Move Packet
			
			m_netModule->Move(l_xmf3Direction.x, l_xmf3Direction.z, xmf2MouseMovement.x,player->m_timeElapsed);
		}
	}
}



void NetInputManagerComponent::Update(float fTimeElapsed)
{
	memcpy(m_pNetKey, m_NotBuffer, sizeof(m_pNetKey));
}

bool NetInputManagerComponent::IsKeyDown(KeyCode key)
{
	return (m_pNetKey[key] & 0xF0);
}

bool NetInputManagerComponent::IsKeyUp(KeyCode key)
{
	return !(m_pNetKey[key] & 0xF0);
}

const XMFLOAT2& NetInputManagerComponent::GetMouseMovement()
{
	return m_xmf2NetMouse;
}

void NetInputManagerComponent::Move(float x, float y, float z, float rx, float ry, float rz)
{
	
	if (0 < x)
	{
		m_pNetKey[KeyCode::_D] = KEYDOWN;
	}
	if (0 > x)
	{
		m_pNetKey[KeyCode::_A] = KEYDOWN;
	}
	if (0 == x)
	{
		m_pNetKey[KeyCode::_D] = m_pNetKey[KeyCode::_A] = !KEYDOWN;
	}
	if (0 < z)
	{
		m_pNetKey[KeyCode::_W] = KEYDOWN;
	}
	if (0 > z)
	{
		m_pNetKey[KeyCode::_S] = KEYDOWN;
	}
	if (0 == z)
	{
		m_pNetKey[KeyCode::_W] = m_pNetKey[KeyCode::_S] = !KEYDOWN;
	}
	
}

void NetInputManagerComponent::Test()
{

}
