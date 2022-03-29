#pragma once

class Character
{
public:
	Character(int maxHp, bool bAutoRevive = false);
	virtual ~Character();

public:
	virtual void Update(float fTimeElapsed);

public:
	void Damage(int);
	bool isAlive() const { return !m_bDead; }
	virtual void Die();
	virtual void Revive();

private:
	virtual bool isDead() const { return 0 >= m_currHp; }

private:
	const int	m_maxHp;
	int			m_currHp;
	const float m_fReviveTime;
	float		m_fCurrReviveTime;
	bool		m_bDead;
	bool		m_bAutoRevive;
};

