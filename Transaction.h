#pragma once

//enum TransactionVariable {
//	JUST,
//	HP,
//	SPD,
//	TIME,
//	REMAIN_TIME_TO_LANDING
//};
//
//using TV = TransactionVariable;
//
//inline bool GreaterThan(float x, float y)			{ return x > y; }
//inline bool GreaterThanEqualsTo(float x, float y)	{ return x >= y; }
//inline bool EqualsTo(float x, float y)				{ return x == y; }
//inline bool LessThanEqualsTo(float x, float y)		{ return x <= y; }
//inline bool LessThan(float x, float y)				{ return x < y; }
//
//using something = bool(*)(float, float);
//
//class State;
//class Object;
//
//class Transaction {
//public:
//	Transaction(State* targetState, TransactionVariable keyward, something foo, float value)
//		: m_targetState(targetState)
//		, target(keyward)
//		, comparisonFuncPtr(foo)
//		, value(value)
//		, time(0.0)
//	{}
//	bool const Satisfy(Object* obj);
//public:
//	State* m_targetState = NULL;
//private:
//	TransactionVariable target;
//	something			comparisonFuncPtr;
//	float				value;
//	double				time;
//};