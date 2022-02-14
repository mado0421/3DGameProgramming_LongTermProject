#include "stdafx.h"
#include "Character.h"

Character::Character(int maxHp)
	: m_maxHp(maxHp)
	, m_currHp(maxHp)
	, m_fReviveTime(3.0f)
	, m_fCurrReviveTime(0.0f)
	, m_bDead(false)
{
}

Character::~Character()
{
}

void Character::Update(float fTimeElapsed)
{
	if (isDead()) Die();

	if (m_bDead) {
		m_fCurrReviveTime += fTimeElapsed;
		if (m_fReviveTime <= m_fCurrReviveTime) Revive();
	}
}

void Character::Damage(int dmg)
{
	m_currHp -= dmg;
}

void Character::Die()
{
	m_bDead = true;
}

void Character::Revive()
{
	m_currHp = m_maxHp;
	m_fCurrReviveTime = 0.0f;
	m_bDead = false;
}
