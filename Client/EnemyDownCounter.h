#pragma once
#include "Component.h"

class EnemyDownCounter : public Component
{
public:
	EnemyDownCounter() = delete;
	EnemyDownCounter(Object* pObject);
	~EnemyDownCounter();

	void AddTarget(Object* pObject);
	void SetTextRenderer(TextRendererComponent* pTRC);

	virtual void Update(float fTimeElapsed);

private:
	vector<pair<Character*, bool>> m_vecTargetCharacter;
	TextRendererComponent* m_pTRC = nullptr;
	int m_count = 0;
};

