#pragma once
#include "Component.h"

class TextUIAmmoComponent : public Component
{
public:
	TextUIAmmoComponent() = delete;
	TextUIAmmoComponent(Object* pObject, Object* pPlayer);
	~TextUIAmmoComponent();

	virtual void Update(float fTimeElapsed);
private:
	TextRendererComponent* m_pTRC;
	HumanoidControllerComponent* m_pHCC;
};

