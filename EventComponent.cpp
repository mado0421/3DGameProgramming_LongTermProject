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
		// ��� ���� ���� �ʱ���·�
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
		// ���� ����, ���� ��ǥ ���·�
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
		// ���� �� ������ �����ϰ� ���� �ٽ� ����

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
	�� �ݶ��̴�(Ʈ����)�� �����ų�, ���� �� �װų�, �ٸ� �̺�Ʈ�� �Ѱų�
	*/

	if (!m_bEnabled) return;

	if (m_EventState == EventState::On) {
		for (int i = 0; i < m_vecSpawnTarget.size(); i++) {
			if (m_vecSpawnTarget[i]->isAlive()) return;
		}
		SetEvent(EventState::Off);
	}


}
