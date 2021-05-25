#include "stdafx.h"
#include "Transaction.h"
#include "Object.h"

//bool const Transaction::Satisfy(Object* obj)
//{
//	switch (target)	{
//	case JUST:	return true;
//	case HP:	return false/*(comparisonFuncPtr(obj.hp, value))*/;
//	case SPD:	return (comparisonFuncPtr(Vector3::Length( obj->GetVelocity() ), value));
//	case TIME:	return (comparisonFuncPtr(time, value));
//	case REMAIN_TIME_TO_LANDING:
//	default:break;	}
//	return false;
//}