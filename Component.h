#pragma once

class Object;

class Component
{
public:
	Component() = delete;
	Component(Object* pObject, const char* strName);
	~Component();

	virtual Component* GetInstance();
	bool isEqualTo(const char* strName);

	virtual void InputEvent(UCHAR* pKeyBuffer) = 0;
	virtual void Update(float fTimeElapsed) = 0;
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList) = 0;

	void SetActive(bool bActive);


protected:
	const string	m_strName	= "";
	Object*			m_pObject	= nullptr;
	bool			m_bEnabled	= true;

public:
};

