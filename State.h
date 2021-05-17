#pragma once

class Transaction;
class Object;

constexpr float walkSpd = 1.5;
constexpr float runSpd = 8;

class State {
public:
	State(const char* strName, Object* obj) :m_strStateName(strName), m_pObject(obj) {}

	void AddTransation(Transaction* transation) { m_vecTransactions.push_back(transation); }
	bool IsSatisfyTransaction(const char* strStateName);
	void ChangeStateTo(const char* strStateName);

	virtual void EnterState() {}
	virtual void LeaveState() {}
public:
	virtual void Update(float fTimeElapsed) = 0;
	virtual void Input(UCHAR* pKeyBuffer) = 0;
	virtual vector<pair<string, float>> GetAnimClipNameList() = 0;

public:
	string m_strStateName;

protected:
	vector<Transaction*>	m_vecTransactions;
	Object*					m_pObject;
};

class HumanoidState_Idle : public State {
public:
	HumanoidState_Idle(const char* strName, Object* obj) :State(strName, obj) {}

	virtual void Update(float fTimeElapsed) {};
	virtual void Input(UCHAR* pKeyBuffer) {
		if (IsKeyDown(pKeyBuffer, KeyCode::_W) || 
			IsKeyDown(pKeyBuffer, KeyCode::_A) || 
			IsKeyDown(pKeyBuffer, KeyCode::_S) || 
			IsKeyDown(pKeyBuffer, KeyCode::_D))
			if (IsSatisfyTransaction("HumanoidState_Moving")) { 
				ChangeStateTo("HumanoidState_Moving"); 
			}
	}
	virtual vector<pair<string, float>> GetAnimClipNameList() {
		vector<pair<string, float>> result;
		result.push_back(pair<string, float>("Humanoid_Idle", 1));
		return result;
	}
};

class HumanoidState_Moving : public State {
public:
	HumanoidState_Moving(const char* strName, Object* obj) 
		:State(strName, obj) 
		, m_xmf3MovingDir(XMFLOAT3(0, 0, 0))
	{}

	virtual void EnterState() { m_bKeyDown = true; m_xmf3MovingDir = XMFLOAT3(0, 0, 0); }
	virtual void Update(float fTimeElapsed);
	virtual void Input(UCHAR* pKeyBuffer);
	virtual vector<pair<string, float>> GetAnimClipNameList();
public:
	bool m_bKeyDown		= false;
	bool m_bShiftDown	= false;
	float m_fSpdIncreaseFactor = 0.8f;
	float m_fDragFactor = 0.13f;
	XMFLOAT3 m_xmf3MovingDir;
};