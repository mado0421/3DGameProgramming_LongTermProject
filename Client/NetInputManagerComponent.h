#pragma once
#include "InputManagerComponent.h"

class ClientWsaModule;
class NetInputManagerComponent :
	public InputManagerComponent
{
	ClientWsaModule* m_netModule;

	UCHAR		m_NotBuffer[256];
	UCHAR		m_pNetKey[256];
	XMFLOAT2	m_xmf2NetMouse;

	bool playerControl = false;
public:
	NetInputManagerComponent() = delete;
	NetInputManagerComponent(Object* pObject, ClientWsaModule* cwm, bool control=false);
	~NetInputManagerComponent();

	virtual void InputEvent(UCHAR* pKeyBuffer, XMFLOAT2& xmf2MouseMovement);
	virtual void Update(float fTimeElapsed);
	virtual bool IsKeyDown(KeyCode key);
	virtual bool IsKeyUp(KeyCode key);
	virtual const XMFLOAT2& GetMouseMovement();

	void Move(float x, float y, float z, float rx, float ry, float rz);

	void Test();
};

