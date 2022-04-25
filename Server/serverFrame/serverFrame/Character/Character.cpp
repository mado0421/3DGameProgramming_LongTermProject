#include"../Headers/constants.h"
#include"../Headers/headers.h"
#include"../DirectX/TransformComponent.h"
#include "Character.h"

Character::Character()
{
	m_currHp = maxHp;
	m_currReviveTime = 0;
	m_bDead = false;
	m_bAutoRevive = true;
	rx = ry = rz = 0;
	transform = new TransformComponent();
	transform->SetPosition(-3, 0, -3);
	transform->RotateXYZDegree(0, 180, 0);
	ry = 180;
	m_xmf3Velocity = XMFLOAT3(0, 0, 0);
	m_fDragFactor = 1.0f;
}

Character::~Character()
{
	delete transform;
}

// for Test debug only
void Character::PrintAll()
{
#if _DEBUG
	cout << "hp: " << m_currHp << "\n";
	cout << "revTime: " << m_currReviveTime<< "\n";
	auto v = transform->GetPosition();
	cout << "position: (" << v.x << " , " << v.y << " , " << v.z << " )\n";
	cout << "rotate: (" << rx << " , " << ry << " , " << rz << " )\n";
#endif
}

void Character::PrintPos()
{
	auto v = transform->GetPosition();
	cout << "position: (" << v.x << " , " << v.y << " , " << v.z << " )\n";
	cout << "rotate: (" << rx << " , " << ry << " , " << rz << " )\n";
}
