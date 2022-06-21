#pragma once
#include "Component.h"

class DoorComponent : public Component
{
public:
	DoorComponent() = delete;
	DoorComponent(Object* pObject, bool bOpen = false);
	~DoorComponent();

	void Open();
	void Close();

	virtual void Update(float fTimeElapsed);

private:
	bool m_bOpen;
	float m_fProgress;
	XMFLOAT3 m_xmf3OrigPosition;
	TransformComponent* l_transform;
};

