#include "stdafx.h"
#include "Component.h"
#include "Object.h"


Component::Component(Object* pObject, const char* strName)
    : m_pObject(pObject)
    , m_strName(strName)
{
}
Component::~Component()
{
}

Component* Component::GetInstance()
{
    return this;
}
bool Component::isEqualTo(const char* strName)
{
    return (strcmp(strName, m_strName.c_str()) == 0);
}

void Component::SetActive(bool bActive)
{
    m_bEnabled = bActive;
}
