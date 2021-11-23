#pragma once

class Object;

class Component
{
public:
	Component() = delete;
	Component(Object* pObject, const char* strName);
	~Component();

	virtual Component* GetInstance();
	bool isEqualTo(const char* strName);

protected:
	Object* m_pObject = nullptr;
	const string m_strName	= "";
};

