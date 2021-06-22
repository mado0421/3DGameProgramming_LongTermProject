#pragma once

class AnimatedObject;
class BoneMask;
class State;
class StateLayer;

using ClipPair = vector<pair<string, float>>;

struct Transition {
	float	m_fTime = 0;
	float	m_fDelay = 0;
	bool	m_bInterruptedCancel = false;
	State*	m_pTargetState = NULL;

	Transition(State* pState) :m_pTargetState(pState) {}
};

class State {
public:
	State(AnimatedObject* pObj, StateLayer* pLayer)
		:m_fTime(0), m_pObj(pObj), m_pLayer(pLayer) {}

public:
	virtual void Enter() { cout << "Enter\n"; }
	virtual void Leave() { cout << "Leave\n"; }
	virtual void Update(float fTimeElapsed) {}
	virtual void Input( UCHAR* pKeyBuffer) {}
	virtual XMVECTOR* GetRotation() { return NULL; }

public:
	float				m_fTime;
protected:
	StateLayer*			m_pLayer = NULL;
	AnimatedObject*		m_pObj = NULL;

public: // for Test
	float	m_fTimeToTransition;
	bool	m_bTransition = false;
	float	m_fDelay;
};

class StateLayer {
public:
	void Update(float fTimeElapsed);
	void Input(UCHAR* pKeyBuffer);
	void Animate(AnimatedObject* pObj);
	void ChangeStateTo(State* pState);

public:
	float		m_fWeight = 1.0f;
	BoneMask*	m_pMask = NULL;

private:
	State*		m_pState = NULL;
};

/*
	Humanoid State
	\Base Layer
	\Action Layer

	Base Layer
	- Idle
	- (Walk)
	- (Run)
	- Jump
	- RunJump

	Action Layer
	- Stand
	- Aim
	- Shoot
	- Reload
	- Skill(not yet)

	Interrupt Layer(not yet)
	- Damage
	- Stuck
	- Stun
	- Down
	- Die

	AnimClip이 없는 State는 아무 것도 Blend 하지 않아야 하는데
	그걸 어떻게 처리할 것인지?
	문자열 ""인지 비교하는건 에바니까 따로 bool변수를 하나 둬서
	m_bIsAnimBlend 같이 해버리는 것도 괜찮을 듯
*/

/*
* Idle은 Walk, Run과 AnimClip을 공유함
* BaseAnimClip은 Idle
* 전환될 때, 키프레임이 갑자기 변해선 안되므로 시간을 전달받아야 함.
* 모든 내용이 동일하고 수렴 속력만 다름.
*/
class StateHumanoidIdle : public State {
public:
	StateHumanoidIdle(AnimatedObject* pObj, StateLayer* pLayer);

public:
	virtual void Enter() {}
	virtual void Leave() {}
	virtual void Update(float fTimeElapsed);
	virtual void Input(UCHAR* pKeyBuffer);
	virtual XMVECTOR* GetRotation();
private:
	XMFLOAT3	m_xmf3InputDir = XMFLOAT3(0, 0, 0);
	float		m_fDragFactor = 1.0f;
};

class StateHumanoidStand : public State {
public:
	StateHumanoidStand(AnimatedObject* pObj, StateLayer* pLayer);

public:
	virtual void Enter() {}
	virtual void Leave() {}
	virtual void Update(float fTimeElapsed);
	virtual void Input(UCHAR* pKeyBuffer);
	virtual XMVECTOR* GetRotation();
};

class StateHumanoidAim : public State {
public:
	StateHumanoidAim(AnimatedObject* pObj, StateLayer* pLayer);

public:
	virtual void Enter() {}
	virtual void Leave() {}
	virtual void Update(float fTimeElapsed);
	virtual void Input(UCHAR* pKeyBuffer);
	virtual XMVECTOR* GetRotation();
};

//class HumanoidObject;
//class BoneMask;
//
//class State {
//public:
//	virtual void EnterState() {}
//	virtual void LeaveState() {}
//
//	virtual void Update(float fTimeElapsed) = 0;
//	virtual void Input(UCHAR* pKeyBuffer) = 0;
//
//protected:
//	bool m_bEnable = false;
//};
//
//class HumanoidState : public State {
//public:
//	HumanoidState(HumanoidObject* pObject)
//		:m_pObject(pObject)
//	{}
//
//	virtual void EnterState() { m_fTime = 0; m_bEnable = true; }
//	virtual void LeaveState() { m_bEnable = false; }
//
//	virtual void MakeAnimTransform() = 0;
//	virtual void BlendAnimTransform() = 0;
//	void AddSubState(const char* strStateName);
//	void AddAction(const char* strStateName);
//	void DisableSubState(const char* strStateName);
//	bool IsEnable() { return m_bEnable; }
//	void SetEnable(const bool bEnable) { m_bEnable = bEnable; }
//protected:
//	HumanoidObject* m_pObject = NULL;
//	BoneMask* m_pBoneMask = NULL;
//	float m_fTime = 0;
//};
//
//class HumanoidBaseIdle : public HumanoidState {
//public:
//	HumanoidBaseIdle(HumanoidObject* pObject)
//		:HumanoidState(pObject)
//	{}
//
//	virtual void Update(float fTimeElapsed);
//	virtual void Input(UCHAR* pKeyBuffer);
//	virtual void MakeAnimTransform();
//	virtual void BlendAnimTransform() {}
//private:
//	XMFLOAT3 m_xmf3InputDir = XMFLOAT3(0,0,0);
//	float m_fDragFactor = 1.0f;
//	bool m_bPrevAiming = false;
//	bool m_bCurrAiming = false;
//};
//
//class HumanoidSubMoving : public HumanoidState {
//public:
//	HumanoidSubMoving(HumanoidObject* pObject);
//
//	virtual void Update(float fTimeElapsed);
//	virtual void Input(UCHAR* pKeyBuffer) {}
//	virtual void MakeAnimTransform() {}
//	virtual void BlendAnimTransform();
//};
//
//class HumanoidSubAiming : public HumanoidState {
//public:
//	HumanoidSubAiming(HumanoidObject* pObject);
//
//	virtual void Update(float fTimeElapsed);
//	virtual void Input(UCHAR* pKeyBuffer);
//	virtual void MakeAnimTransform() {}
//	virtual void BlendAnimTransform();
//};