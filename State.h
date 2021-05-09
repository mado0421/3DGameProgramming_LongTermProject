#pragma once

class Transaction;
class Object;

constexpr float walkSpd = 1.38;
constexpr float runSpd = 10;

class State {
public:
	State(const char* strName, Object* obj) :m_strStateName(strName), m_pObject(obj) {}

	void AddTransation(Transaction* transation) { m_vecTransactions.push_back(transation); }
	bool IsSatisfyTransaction(const char* strStateName);
	void ChangeStateTo(const char* strStateName);

	virtual void EnterState() {}
	virtual void LeaveState() {}
public:
	virtual void MoveForward() = 0;
	virtual void Update(float fTimeElapsed) = 0;

public:
	string m_strStateName;

protected:
	vector<Transaction*>	m_vecTransactions;
	Object*					m_pObject;
};

class HumanoidState_Idle : public State {
public:
	HumanoidState_Idle(const char* strName, Object* obj) :State(strName, obj) {}

	virtual void MoveForward() { if (IsSatisfyTransaction("Humanoid_Walk")) { ChangeStateTo("Humanoid_Walk"); } };
	virtual void Update(float fTimeElapsed) {};
};

class HumanoidState_Walk : public State {
public:
	HumanoidState_Walk(const char* strName, Object* obj) :State(strName, obj) {}

	virtual void EnterState() { m_bKeyDown = true; }
	virtual void MoveForward() { m_bKeyDown = true; };
	virtual void Update(float fTimeElapsed);

public:
	bool m_bKeyDown = false;
	float m_fSpdIncreaseFactor = 1.0f;
	float m_fSpdDecreaseFactor = 1.8f;
	float m_fSpdMaximum = 5.0f;
};