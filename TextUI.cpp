#include "stdafx.h"
#include "Components.h"

TextUIAmmoComponent::TextUIAmmoComponent(Object* pObject, Object* pPlayer)
	:Component(pObject)
{
	TextRendererComponent* temp = pObject->FindComponent<TextRendererComponent>();
	if (temp) m_pTRC = temp;

	m_pHCC = pPlayer->FindComponent<HumanoidControllerComponent>();
}

TextUIAmmoComponent::~TextUIAmmoComponent()
{
}

void TextUIAmmoComponent::Update(float fTimeElapsed)
{
	int maxAmmo, curAmmo;
	//maxAmmo = m_pHCC->m_maxAmmo;
	//curAmmo = m_pHCC->m_curAmmo;

	bool isReloading;
	//isReloading = m_pHCC->m_bReloading;

	string text = "Ammo: ";

	//if (isReloading) {
	//	text += "Reload";
	//}
	//else {
	//	text += to_string(curAmmo);
	//	text += "/";
	//	text += to_string(maxAmmo);
	//}
	//m_pTRC->SetText(text.c_str());

}
