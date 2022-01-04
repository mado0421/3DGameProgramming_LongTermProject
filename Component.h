#pragma once

class Object;

class Component
{
public:
	Component() = delete;
	Component(Object* pObject);
	~Component();

	virtual Component* GetInstance();

	virtual void SolveConstraint() {}
	virtual void Update(float fTimeElapsed) {}
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList) {}

	void SetActive(bool bActive);

public:
	Object*			m_pObject	= nullptr;
	bool			m_bEnabled	= true;
};

