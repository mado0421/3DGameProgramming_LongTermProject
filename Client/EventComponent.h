#pragma once
#include "Component.h"

class EventComponent : public Component
{
public:
	enum class EventState {
		Ready, On, Off
	};

	EventComponent() = delete;
	EventComponent(Object* pObject, EventState eventState = EventState::Ready);
	~EventComponent();

	void SetEvent(EventState eventState);

	void AddSpawn(Character* pCharacter);
	void AddDoorOpen(DoorComponent* pDoor);
	void AddDoorClose(DoorComponent* pDoor);

	virtual void SolveConstraint();
	virtual void Update(float fTimeElapsed);

private:
	vector< Character* > m_vecSpawnTarget;
	vector< DoorComponent* > m_vecDoorForOpen;
	vector< DoorComponent* > m_vecDoorForClose;
	EventState m_EventState;
};

