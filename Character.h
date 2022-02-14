#pragma once

class Character
{
public:
	Character(int maxHp);
	virtual ~Character();

public:
	virtual void Update(float fTimeElapsed);

public:
	void Damage(int);
	bool isAlive() const { return !m_bDead; }

private:
	virtual bool isDead() const { return 0 >= m_currHp; }
	virtual void Die();
	virtual void Revive();

private:
	const int	m_maxHp;
	int			m_currHp;
	const float m_fReviveTime;
	float		m_fCurrReviveTime;
	bool		m_bDead;
};

