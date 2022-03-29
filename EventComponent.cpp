#include "stdafx.h"
#include "Components.h"

EventComponent::EventComponent(Object* pObject, EventState eventState)
	:Component(pObject)
	, m_EventState(eventState)
{
}

EventComponent::~EventComponent()
{
}

void EventComponent::SetEvent(EventState eventState)
{
	switch (eventState)
	{
	case EventComponent::EventState::Ready:
		// 모든 적과 문을 초기상태로
		for_each(m_vecSpawnTarget.begin(),
			m_vecSpawnTarget.end(),
			[&](Character* c) {c->Die(); });

		for_each(m_vecDoorForOpen.begin(),
			m_vecDoorForOpen.end(),
			[&](DoorComponent* d) {d->Close(); });

		for_each(m_vecDoorForClose.begin(),
			m_vecDoorForClose.end(),
			[&](DoorComponent* d) {d->Open(); });

		break;
	case EventComponent::EventState::On:
		// 적을 스폰, 문을 목표 상태로
		for_each(m_vecSpawnTarget.begin(),
			m_vecSpawnTarget.end(),
			[&](Character* c) {c->Revive(); });

		for_each(m_vecDoorForOpen.begin(),
			m_vecDoorForOpen.end(),
			[&](DoorComponent* d) {d->Open(); });

		for_each(m_vecDoorForClose.begin(),
			m_vecDoorForClose.end(),
			[&](DoorComponent* d) {d->Close(); });

		break;
	case EventComponent::EventState::Off:
		// 적이 다 죽은걸 가정하고 문을 다시 열음

		for_each(m_vecDoorForOpen.begin(),
			m_vecDoorForOpen.end(),
			[&](DoorComponent* d) {d->Open(); });

		for_each(m_vecDoorForClose.begin(),
			m_vecDoorForClose.end(),
			[&](DoorComponent* d) {d->Open(); });
		break;
	default:
		break;
	}

	m_EventState = eventState;
}

void EventComponent::AddSpawn(Character* pCharacter)
{
	m_vecSpawnTarget.push_back(pCharacter);
}

void EventComponent::AddDoorOpen(DoorComponent* pDoor)
{
	m_vecDoorForOpen.push_back(pDoor);
}

void EventComponent::AddDoorClose(DoorComponent* pDoor)
{
	m_vecDoorForClose.push_back(pDoor);
}

void EventComponent::SolveConstraint()
{
	vector<ColliderComponent*> l_vecCollider = m_pObject->FindComponents<ColliderComponent>();

	if (m_EventState == EventState::Ready) {
		for_each(l_vecCollider.begin(), l_vecCollider.end(), [&](ColliderComponent* c) {
			for (int i = 0; i < c->m_vecpCollided.size(); i++) {
				HumanoidControllerComponent* hcc = c->m_vecpCollided[i]->m_pObject->FindComponent<HumanoidControllerComponent>();
				if (hcc)
					SetEvent(EventState::On);
			}
			});
	}
}

void EventComponent::Update(float fTimeElapsed)
{
	/*
	내 콜라이더(트리거)에 잡히거나, 적이 다 죽거나, 다른 이벤트가 켜거나
	*/

	if (!m_bEnabled) return;

	if (m_EventState == EventState::On) {
		for (int i = 0; i < m_vecSpawnTarget.size(); i++) {
			if (m_vecSpawnTarget[i]->isAlive()) return;
		}
		SetEvent(EventState::Off);
	}


}
