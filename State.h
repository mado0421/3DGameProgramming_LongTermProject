#pragma once

class HumanoidObject;
class BoneMask;

class HumanoidState {
public:
	HumanoidState(HumanoidObject* pObject)
		:m_pObject(pObject)
	{}

	virtual void EnterState() { m_fTime = 0; m_bEnable = true; }
	virtual void LeaveState() { m_bEnable = false; }

	virtual void Update(float fTimeElapsed) = 0;
	virtual void Input(UCHAR* pKeyBuffer) = 0;
	virtual void MakeAnimTransform() = 0;
	virtual void BlendAnimTransform() = 0;
	void AddSubState(const char* strStateName);
	void AddAction(const char* strStateName);
	void DisableSubState(const char* strStateName);
	bool IsEnable() { return m_bEnable; }
	void SetEnable(const bool bEnable) { m_bEnable = bEnable; }
protected:
	HumanoidObject* m_pObject = NULL;
	BoneMask* m_pBoneMask = NULL;
	float m_fTime = 0;
	bool m_bEnable = false;
};

class HumanoidBaseIdle : public HumanoidState {
public:
	HumanoidBaseIdle(HumanoidObject* pObject)
		:HumanoidState(pObject)
	{}

	virtual void Update(float fTimeElapsed);
	virtual void Input(UCHAR* pKeyBuffer);
	virtual void MakeAnimTransform();
	virtual void BlendAnimTransform() {}
private:
	XMFLOAT3 m_xmf3InputDir = XMFLOAT3(0,0,0);
	float m_fDragFactor = 1.0f;
	bool m_bPrevAiming = false;
	bool m_bCurrAiming = false;
};

class HumanoidSubMoving : public HumanoidState {
public:
	HumanoidSubMoving(HumanoidObject* pObject);

	virtual void Update(float fTimeElapsed);
	virtual void Input(UCHAR* pKeyBuffer) {}
	virtual void MakeAnimTransform() {}
	virtual void BlendAnimTransform();
};

class HumanoidSubAiming : public HumanoidState {
public:
	HumanoidSubAiming(HumanoidObject* pObject);

	virtual void Update(float fTimeElapsed);
	virtual void Input(UCHAR* pKeyBuffer);
	virtual void MakeAnimTransform() {}
	virtual void BlendAnimTransform();
};




//
//class Transaction;
//class HumanoidObject;
//
//constexpr float walkSpd = 1.5;
//constexpr float runSpd = 8;



//
//class State {
//public:
//	State(const char* strName, Object* obj) :m_strStateName(strName), m_pObject(obj) {}
//
//	void AddTransation(Transaction* transation) { m_vecTransactions.push_back(transation); }
//	bool IsSatisfyTransaction(const char* strStateName);
//	void ChangeStateTo(const char* strStateName);
//
//	virtual void EnterState() {}
//	virtual void LeaveState() {}
//public:
//	virtual void Update(float fTimeElapsed) = 0;
//	virtual void Input(UCHAR* pKeyBuffer) = 0;
//	virtual vector<pair<string, float>> GetAnimClipNameList() = 0;
//
//public:
//	string m_strStateName;
//
//protected:
//	vector<Transaction*>	m_vecTransactions;
//	Object*					m_pObject;
//};
//
//class HumanoidState_Idle : public State {
//public:
//	HumanoidState_Idle(const char* strName, Object* obj) :State(strName, obj) {}
//
//	virtual void Update(float fTimeElapsed) {};
//	virtual void Input(UCHAR* pKeyBuffer) {
//		if (IsKeyDown(pKeyBuffer, KeyCode::_W) || 
//			IsKeyDown(pKeyBuffer, KeyCode::_A) || 
//			IsKeyDown(pKeyBuffer, KeyCode::_S) || 
//			IsKeyDown(pKeyBuffer, KeyCode::_D))
//			if (IsSatisfyTransaction("HumanoidState_Moving")) { 
//				ChangeStateTo("HumanoidState_Moving"); 
//			}
//	}
//	virtual vector<pair<string, float>> GetAnimClipNameList() {
//		vector<pair<string, float>> result;
//		result.push_back(pair<string, float>("Humanoid_Idle", 1));
//		return result;
//	}
//};
//
//class HumanoidState_Moving : public State {
//public:
//	HumanoidState_Moving(const char* strName, Object* obj) 
//		:State(strName, obj) 
//		, m_xmf3InputDir(XMFLOAT3(0, 0, 0))
//	{}
//
//	virtual void EnterState() { m_bKeyDown = true; m_xmf3InputDir = XMFLOAT3(0, 0, 0); }
//	virtual void Update(float fTimeElapsed);
//	virtual void Input(UCHAR* pKeyBuffer);
//	virtual vector<pair<string, float>> GetAnimClipNameList();
//public:
//	bool m_bKeyDown		= false;
//	bool m_bShiftDown	= false;
//	float m_fDragFactor = 1.0f;
//	XMFLOAT3 m_xmf3InputDir;
//};
//

/*
	State, SubState, Action

	- State: 키입력 처리, 기본적인 Animation 출력
	- SubState: 특정 변수 조정, State Animation을 Overwrite
	- Action: 상위 Animation을 Overwrite

	Mask Struct도 있어야 함.
	BoneMask::eUpperBody
	BoneMask::eLowerBody
	기본적으론 Bone의 Idx를 갖고 있는 배열
	여기선 Pair로 처리해야 하나?
	Mask가 가져야 할 정보: 몇 번 Bone에 얼마의 값. (스텐실 처럼)

	애니메이션은 BaseState에서 먼저 애니메이션을 계산.	m_AnimCtrl->CalcAnimTransformFromBaseState(vecPairClipWeight, baseStateTime);
	그 다음, SubState에서 계산된 애니메이션에 Blend.	m_AnimCtrl->BlendToPrevAnimTransform(vecPairClipWeight, time, Mask);
	추가로 Action이 있다면 위의 결과값에 추가로 Blend.	m_AnimCtrl->BlendToPrevAnimTransform(vecPairClipWeight, time, Mask);

	State는 한 순간에 단 하나만 존재.
	State간 전환은 Object의 상태를 보고 State에서 판단.	if(IsPossibleToChangeStatdTo(strStateName)) ChangeStateTo(strStateName);
	SubState의 추가 및 시간 갱신은 State에서 판단.			if(IsPossibleToAddSubState(strSubStateName))AddSubState(strSubStateName);
	Action의 추가 및 시간 갱신은 State에서 판단.			if(IsPossibleToAddAction(strActionName))	AddAction(strActionName);

	SubState는 여러 종류가 한 순간에 존재 할 수 있음.
	하지만 이미 존재하는 SubState가 추가로 존재할 수는 없음.
	이미 존재하는 SubState를 추가할 때에는 해당 SubState의 시간을 갱신함. 또는 아무 것도 하지 않음. (애초에 SubState는 유지되는 하위 State니까)

	Action 또한 SubState와 같음.

	HumanoidObject는 다음과 같은 변수를 가짐.
	m_CurrState, m_vecCurrSubState, m_vecCurrAction;

	State->GetPairClipWeight()로 Clip이랑 Weight Pair를 넘기지 말고 그냥 Update()에서 처리해버리거나 Render()에서 처리해버리거나 할 것.
	그래야 SubState, Action에서도 일관성 있게 처리할 수 있음.

*/

//class BoneMask;
//class BaseState;
//extern unordered_map<string, BaseState*> g_uomStates;
//
//class BaseState {
//public:
//	enum class baseState {
//		eIDLE = 0,
//		eJUMP,
//		eDIE
//	};
//	enum class subState {
//		eMOVING = 0,
//		eAIMING,
//		eINAIR
//	};
//	enum class action {
//		eSHOOTING = 0
//	};
//public:
//	BaseState() = default;
//
//
//	virtual void EnterState() {}
//	virtual void LeaveState() {}
//
//	virtual void Update(float fTimeElapsed) = 0;
//	virtual void PrepareRender(ID3D12GraphicsCommandList* pd3dCommandList) = 0;
//	virtual void InputProcess(UCHAR* pKeyBuffer) = 0;
//
//private:
//	virtual void ChangeStateTo(baseState type) = 0;
//	virtual void AddSubState(subState type) = 0;
//	virtual void AddAction(action type) = 0;
//
//protected:
//	float m_fTime = 0;
//	BoneMask* m_pBoneMask = NULL;
//};
//
//class HumanoidBaseState : public BaseState {
//public:
//	HumanoidBaseState(HumanoidObject* pObject);
//
//	virtual void Update(float fTimeElapsed) = 0;
//	virtual void PrepareRender(ID3D12GraphicsCommandList* pd3dCommandList) = 0;
//	virtual void InputProcess(UCHAR* pKeyBuffer) = 0;
//
//private:
//	virtual void ChangeStateTo(baseState type) = 0;
//	virtual void AddSubState(subState type) = 0;
//	virtual void AddAction(action type) = 0;
//
//protected:
//	HumanoidObject* m_pObject = NULL;
//};
//class HumanoidSubState : public HumanoidBaseState {
//
//public:
//	HumanoidSubState() = delete;
//	HumanoidSubState(HumanoidObject* pObject) :HumanoidBaseState(pObject) {}
//
//	virtual void Update(float fTimeElapsed) = 0;
//	virtual void PrepareRender(ID3D12GraphicsCommandList* pd3dCommandList) = 0;
//	virtual void InputProcess(UCHAR* pKeyBuffer) = 0;
//
//public:
//
//};
//class HumanoidAction : public HumanoidBaseState {
//
//public:
//	HumanoidAction() = delete;
//	HumanoidAction(HumanoidObject* pObject) :HumanoidBaseState(pObject) {}
//
//	virtual void Update(float fTimeElapsed) = 0;
//	virtual void PrepareRender(ID3D12GraphicsCommandList* pd3dCommandList) = 0;
//	virtual void InputProcess(UCHAR* pKeyBuffer) {};
//
//public:
//
//};
//
//class HumanoidBaseIdle : public HumanoidBaseState {
//public:
//	HumanoidBaseIdle(HumanoidObject* pObject);
//
//	virtual void Update(float fTimeElapsed);
//	virtual void InputProcess(UCHAR* pKeyBuffer);
//	virtual void PrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
//
//private:
//	virtual void ChangeStateTo(baseState type);
//	virtual void AddSubState(subState type);
//	virtual void AddAction(action type);
//private:
//	XMFLOAT3 m_xmf3InputDirection = XMFLOAT3(0, 0, 0);
//	float m_fDragFactor = 1.0f;
//	
//};
//class HumanoidBaseJump : public HumanoidBaseState {
//public:
//	HumanoidBaseJump(HumanoidObject* pObject);
//
//
//	virtual void Update(float fTimeElapsed);
//	virtual void InputProcess(UCHAR* pKeyBuffer);
//	virtual void PrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
//protected:
//	virtual void ChangeStateTo(baseState type);
//	virtual void AddSubState(subState type);
//	virtual void AddAction(action type);
//
//};
//class HumanoidSubAiming : public HumanoidSubState {
//public:
//
//};
//class HumanoidSubMoving : public HumanoidSubState {
//public:
//
//};
//class HumanoidSubInAir : public HumanoidSubState {
//public:
//
//};
//class HumanoidActShooting : public HumanoidAction {
//public:
//
//};
//class HumanoidActDamage : public HumanoidAction {
//public:
//
//};