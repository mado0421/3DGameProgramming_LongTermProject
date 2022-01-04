#include "stdafx.h"
#include "HumanoidControllerComponent.h"
#include "Components.h"
#include "Object.h"

HumanoidControllerComponent::HumanoidControllerComponent(Object* pObject, Object* pWeapon)
	:Component(pObject)
	, m_pWeaponObject(pWeapon)
{
}

HumanoidControllerComponent::~HumanoidControllerComponent()
{
}

void HumanoidControllerComponent::SolveConstraint()
{
	vector<ColliderComponent*> c = m_pObject->FindComponents<ColliderComponent>();

	static int n = 0;

	if (!c.empty()) 
		if(c[0]->m_vecpCollided.size())
		{
			cout << n << " collided!!!\n";
			n++;
		}
}

void HumanoidControllerComponent::Update(float fTimeElapsed)
{
	if (!m_bEnabled) return;

	InputManagerComponent* l_pInput = m_pObject->FindComponent<InputManagerComponent>();
	
	m_fTime += fTimeElapsed;

	/*
	
		하려고 한 것:
		- 무브먼트 파트 먼저 로컬라이징하기
		- 애니메이터에서 받아와서 렌더링하기(StateHumanoidIdle::GetRotation() 여기 내용을 애니메이터에 넣으면 될 듯)

	*/


	// Movement Part
	{
		XMFLOAT3 l_xmf3Direction = XMFLOAT3(0, 0, 0);
		
		if (l_pInput->IsKeyDown(KeyCode::_W)) { l_xmf3Direction.z += m_fSpeed; }
		if (l_pInput->IsKeyDown(KeyCode::_A)) { l_xmf3Direction.x -= m_fSpeed; }
		if (l_pInput->IsKeyDown(KeyCode::_S)) { l_xmf3Direction.z -= m_fSpeed; }
		if (l_pInput->IsKeyDown(KeyCode::_D)) { l_xmf3Direction.x += m_fSpeed; }
		//if (l_pInput->IsKeyDown(KeyCode::_R)) { l_xmf3Direction.y += m_fSpeed; }
		//if (l_pInput->IsKeyDown(KeyCode::_F)) { l_xmf3Direction.y -= m_fSpeed; }

		float fPrevSpd = Vector3::Length(m_xmf3Velocity);
		float fSpdLimit = 1.5f;
		if (!Vector3::IsZero(l_xmf3Direction)) {
			m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::Multiply(fTimeElapsed * 2.8, Vector3::Normalize(l_xmf3Direction)));
			Clamp(m_xmf3Velocity.x, -fSpdLimit, fSpdLimit);
			Clamp(m_xmf3Velocity.z, -fSpdLimit, fSpdLimit);
		}

		if (0 == l_xmf3Direction.x) {
			if (m_xmf3Velocity.x > 0)	m_xmf3Velocity.x -= m_fDragFactor * fTimeElapsed;
			else						m_xmf3Velocity.x += m_fDragFactor * fTimeElapsed;
			if (m_fDragFactor * fTimeElapsed >= fabs(m_xmf3Velocity.x)) m_xmf3Velocity.x = 0;
		}
		if (0 == l_xmf3Direction.z) {
			if (m_xmf3Velocity.z > 0)	m_xmf3Velocity.z -= m_fDragFactor * fTimeElapsed;
			else						m_xmf3Velocity.z += m_fDragFactor * fTimeElapsed;
			if (m_fDragFactor * fTimeElapsed >= fabs(m_xmf3Velocity.z)) m_xmf3Velocity.z = 0;
		}
		m_pObject->FindComponent<TransformComponent>()->Translate(Vector3::Multiply(fTimeElapsed, m_xmf3Velocity));
	}

	// Action Part
	{
		if (l_pInput->IsKeyDown(KeyCode::_LMB)) {
			if (m_fTimeForAim > m_fAimProgress) 
				m_fAimProgress += fTimeElapsed;
			else 
				m_pWeaponObject->FindComponent<WeaponControllerComponent>()->Fire();
		}
		else {
			m_fAimProgress -= fTimeElapsed;
			if (0 > m_fAimProgress) m_fAimProgress = 0;
		}
	}
}
