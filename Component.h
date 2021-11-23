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

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList) = 0;
	virtual void Update(float fTimeElapsed) = 0;
	virtual void InputEvent(UCHAR* pKeyBuffer) = 0;

protected:
	Object* m_pObject = nullptr;
	const string m_strName	= "";
};

