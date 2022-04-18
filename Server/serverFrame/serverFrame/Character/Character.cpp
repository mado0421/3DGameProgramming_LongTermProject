#include "Character.h"

Character::Character()
{
	m_currHp = maxHp;
	m_currReviveTime = 0;
	m_bDead = false;
	m_bAutoRevive = true;
	x = y = z = 0;
	rx = ry = rz = 0;
}

Character::~Character()
{
}

// for Test debug only
void Character::PrintAll()
{
#if _DEBUG
	cout << "hp: " << m_currHp << "\n";
	cout << "revTime: " << m_currReviveTime<< "\n";
	cout << "position: (" << x << " , " << y << " , " << z << " )\n";
	cout << "rotate: (" << rx << " , " << ry << " , " << rz << " )\n";
#endif
}

void Character::PrintPos()
{
	cout << "position: (" << x << " , " << y << " , " << z << " )\n";
	cout << "rotate: (" << rx << " , " << ry << " , " << rz << " )\n";
}
