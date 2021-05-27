#include "stdafx.h"
#include "State.h"
#include "Object.h"
#include "Mask.h"

void HumanoidBaseIdle::Update(float fTimeElapsed)
{
	m_fTime += fTimeElapsed;

	if (!m_pObject) return;

	XMFLOAT3 xmf3Velocity = m_pObject->GetVelocity();
	float fPrevSpd = Vector3::Length(xmf3Velocity);
	float fSpdLimit = 1.5f;
	if (!Vector3::IsZero(m_xmf3InputDir)) {
		xmf3Velocity = Vector3::Add(xmf3Velocity, Vector3::Multiply(fTimeElapsed, Vector3::Normalize(m_xmf3InputDir)));
		Clamp(xmf3Velocity.x, -fSpdLimit, fSpdLimit);
		Clamp(xmf3Velocity.z, -fSpdLimit, fSpdLimit);

		m_pObject->SetVelocity(xmf3Velocity);
	}

	if (0 == m_xmf3InputDir.x) {
		if (xmf3Velocity.x > 0)	xmf3Velocity.x -= m_fDragFactor * fTimeElapsed;
		else					xmf3Velocity.x += m_fDragFactor * fTimeElapsed;
		if (m_fDragFactor * fTimeElapsed >= fabs(xmf3Velocity.x)) xmf3Velocity.x = 0;
	}
	if (0 == m_xmf3InputDir.z) {
		if (xmf3Velocity.z > 0)	xmf3Velocity.z -= m_fDragFactor * fTimeElapsed;
		else					xmf3Velocity.z += m_fDragFactor * fTimeElapsed;
		if (m_fDragFactor * fTimeElapsed >= fabs(xmf3Velocity.z)) xmf3Velocity.z = 0;
	}
	
	m_pObject->SetVelocity(xmf3Velocity);

	m_pObject->Move(Vector3::Multiply(fTimeElapsed, m_pObject->GetVelocity()));

	if (0 == fPrevSpd && 0 < Vector3::Length(xmf3Velocity)) AddSubState("MOVING");
	if (!m_bPrevAiming && m_bCurrAiming) AddSubState("AIMING");
	if (m_bPrevAiming && !m_bCurrAiming) DisableSubState("AIMING");
	
	MakeAnimTransform();
}

void HumanoidBaseIdle::Input(UCHAR* pKeyBuffer)
{
	if (!m_pObject) return;

	// 눌린 방향을 알려준다.
	m_xmf3InputDir = XMFLOAT3(0, 0, 0);
	if (IsKeyDown(pKeyBuffer, KeyCode::_W)) { m_xmf3InputDir.z += 1; }
	if (IsKeyDown(pKeyBuffer, KeyCode::_A)) { m_xmf3InputDir.x -= 1; }
	if (IsKeyDown(pKeyBuffer, KeyCode::_S)) { m_xmf3InputDir.z -= 1; }
	if (IsKeyDown(pKeyBuffer, KeyCode::_D)) { m_xmf3InputDir.x += 1; }

	m_bPrevAiming = m_bCurrAiming;
	if (pKeyBuffer[VK_RBUTTON] & 0xF0) { m_bCurrAiming = true; }
	else m_bCurrAiming = false;

	//if (IsKeyDown(pKeyBuffer, KeyCode::_Shift)) { m_bShiftDown = true; }
}

void HumanoidBaseIdle::MakeAnimTransform()
{
	if (!m_pObject) return;

	vector<pair<string, float>> result;
	if (result.empty()) result.push_back(pair<string, float>("Humanoid_Idle", 1));

	AnimationCalculate::MakeAnimationTransform(result, m_fTime, m_pObject);
}

void HumanoidState::AddSubState(const char* strStateName)
{
	m_pObject->AddSubState(strStateName);
}

void HumanoidState::AddAction(const char* strStateName)
{
	m_pObject->AddAction(strStateName);
}

void HumanoidState::DisableSubState(const char* strStateName)
{
	m_pObject->QuitSubState(strStateName);
}

HumanoidSubMoving::HumanoidSubMoving(HumanoidObject* pObject)
	:HumanoidState(pObject)
{
	//m_pBoneMask = new BoneMask(BoneMask::PreDefined::eLowerBody, 0);
}

void HumanoidSubMoving::Update(float fTimeElapsed)
{
	if (!m_pObject) return;

	m_fTime += fTimeElapsed;

	if (Vector3::IsZero(m_pObject->GetVelocity())) m_pObject->QuitSubState("MOVING");
}

void HumanoidSubMoving::BlendAnimTransform()
{
	vector<pair<string, float>> result;

	XMFLOAT3 xmf3Velocity = m_pObject->GetVelocity();
	float fVelocityLength = Vector3::Length(xmf3Velocity);

	float idleFactor = (1.5 - fVelocityLength) / 1.5;
	Clamp(idleFactor, 0, 1);
	// 삼각함수는~~~~ 45도일 때 둘이 합쳐서 1이 넘어요~~~~
	if (fVelocityLength) {
		XMFLOAT3 normalizedDir = Vector3::Normalize(xmf3Velocity);

		if (!normalizedDir.x) {
			if (0 < normalizedDir.z) result.push_back(pair<string, float>("Humanoid_WalkingForward", (1 - idleFactor) * 1));
			if (0 > normalizedDir.z) result.push_back(pair<string, float>("Humanoid_WalkingBackward", (1 - idleFactor) * 1));
		}
		else if (!normalizedDir.z) {
			if (0 < normalizedDir.x) result.push_back(pair<string, float>("Humanoid_WalkingRightStrafe", (1 - idleFactor) * 1));
			if (0 > normalizedDir.x) result.push_back(pair<string, float>("Humanoid_WalkingLeftStrafe", (1 - idleFactor) * 1));
		}
		else {
			float x = normalizedDir.x / (abs(normalizedDir.x) + abs(normalizedDir.z));
			float z = normalizedDir.z / (abs(normalizedDir.x) + abs(normalizedDir.z));

			if (0 < z) result.push_back(pair<string, float>("Humanoid_WalkingForward", (1 - idleFactor) * z));
			if (0 > z) result.push_back(pair<string, float>("Humanoid_WalkingBackward", (1 - idleFactor) * -z));

			float forward = normalizedDir.z;
			float strafe = 1 - abs(normalizedDir.z);
			float backward = -normalizedDir.z;

			Clamp(forward, 0, 1);
			Clamp(backward, 0, 1);

			if (0 < x) {
				if (forward) result.push_back(pair<string, float>("Humanoid_WalkingRightStrafeForward", (1 - idleFactor) * x * forward));
				if (strafe)	result.push_back(pair<string, float>("Humanoid_WalkingRightStrafe", (1 - idleFactor) * x * strafe));
				if (backward)result.push_back(pair<string, float>("Humanoid_WalkingLeftStrafeBack", (1 - idleFactor) * x * backward));
			}
			else if (0 > x) {
				if (forward) result.push_back(pair<string, float>("Humanoid_WalkingLeftStrafeForward", (1 - idleFactor) * -x * forward));
				if (strafe)	result.push_back(pair<string, float>("Humanoid_WalkingLeftStrafe", (1 - idleFactor) * -x * strafe));
				if (backward)result.push_back(pair<string, float>("Humanoid_WalkingRightStrafeBack", (1 - idleFactor) * -x * backward));
			}
		}
	}

	if (result.empty()) return;
	AnimationCalculate::BlendToAnimaionTransform(result, m_fTime, m_pObject, m_pBoneMask);
}

HumanoidSubAiming::HumanoidSubAiming(HumanoidObject* pObject)
	:HumanoidState(pObject)
{
	m_pBoneMask = new BoneMask(BoneMask::PreDefined::eUpperBody, 1);
}

void HumanoidSubAiming::Update(float fTimeElapsed)
{
	if (!m_pObject) return;

	m_fTime += fTimeElapsed;
}

void HumanoidSubAiming::Input(UCHAR* pKeyBuffer)
{
}

void HumanoidSubAiming::BlendAnimTransform()
{
	vector<pair<string, float>> result;

	result.push_back(pair<string, float>("Humanoid_Aiming", 1));

	if (result.empty()) return;
	AnimationCalculate::BlendToAnimaionTransform(result, m_fTime, m_pObject, m_pBoneMask);
}
