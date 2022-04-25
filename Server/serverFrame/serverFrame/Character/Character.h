#pragma once

class TransformComponent;
class Character
{
public:
	int m_currHp;
	float m_currReviveTime;
	bool m_bDead;
	bool m_bAutoRevive;
	float rx, ry, rz;
	TransformComponent* transform;
	XMFLOAT3 m_xmf3Velocity;
	float m_fDragFactor;

public:
	Character();
	~Character();

	void PrintAll();
	void PrintPos();
};

