#include "stdafx.h"
#include "Components.h"
#include "Material.h"

ParticleEmitterComponent::ParticleEmitterComponent(Object* pObject, vector<Object*>* particlePool)
	:Component(pObject)
	, m_pSceneParticlePool(particlePool)
	, m_fDuration(0)
	, m_bLooping(true)
	, m_bPrewarm(false)
	, m_fStartDelay(0)
	, m_fStartLifetime(fRange(2, 3))
	, m_fStartSpeed(fRange(1, 1))
	, m_fStartSize(fRange(0.1f, 0.2f))
	, m_xmf3StartRotation(XMFLOAT3(0,0,1))
	, m_fRotationAngle(30)
	, m_fFlipRotation(0)
	, m_fGravityModifier(0.5f)
	, m_RateOverTime(5)
	, m_StopAction(ParticleStopAction::None)
	, m_fCreateCooltime(1.0f / m_RateOverTime)
	, m_fCurrCooltime(0)
{
}

ParticleEmitterComponent::~ParticleEmitterComponent()
{
}

void ParticleEmitterComponent::Update(float fTimeElapsed)
{
	// Update Particle
	// 여기서 erase() 한다고 해서 원본 주소가 지워지거나 하지 않으므로 안심하고 erase() 해도 된다.
	for (auto iter = m_vecParticle.begin(); iter != m_vecParticle.end();) {
		(*iter)->Update(fTimeElapsed);
		if (!(*iter)->m_bEnable) iter = m_vecParticle.erase(iter);
		else iter++;
	}

	if (!m_bEnabled) return;

	// Update Emitter
	
	// for Test!
	if (m_fCreateCooltime < m_fCurrCooltime && 30 > m_vecParticle.size()) {
		m_fCurrCooltime = 0;
		AddParticle();
	}
	m_fCurrCooltime += fTimeElapsed;
	//AddParticle();
}

void ParticleEmitterComponent::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	// m_strMaterialName이 ""면 오류 뱉어야 함~ 잘하자.
	// meshRenderer랑 다르게 particle은 독자적인 render()를 갖추고 있어야 한다.
	// Particle이 material을 갖지 않는다. material Set은 여기서 한 번 하고 그 뒤로 render()만 함.
	g_MaterialMng.SetMaterial(m_strMaterialName.c_str(), pd3dCommandList);
	for_each(m_vecParticle.begin(), m_vecParticle.end(), [&](Object* o) {o->Render(pd3dCommandList); });
}

void ParticleEmitterComponent::SetMaterialByName(const char* strMaterialName)
{
	m_strMaterialName = strMaterialName;
}

void ParticleEmitterComponent::AddParticle()
{
	// Access SceneParticlePool, Get disabled Particle Object, Init them, add to m_vecParticle;
	Object* particle = GetUsableParticle();
	if (particle) {
		InitializeParticle(particle);
		m_vecParticle.push_back(particle);
	}
	else {
		// 생성 못 함~
	}
}

Object* ParticleEmitterComponent::GetUsableParticle()
{
	for (int i = 0; i < m_pSceneParticlePool->size(); i++) {
		if (!(*m_pSceneParticlePool)[i]->m_bEnable)
			return (*m_pSceneParticlePool)[i];
	}
	return nullptr;
}

void ParticleEmitterComponent::InitializeParticle(Object* pObject)
{
	ParticleComponent* pc = pObject->FindComponent<ParticleComponent>();
	TransformComponent* t = m_pObject->FindComponent<TransformComponent>();
	ParticlePropertiesPack ppp;

	ppp.currPos = t->GetPosition();
	ppp.direction = t->GetLookVector();
	ppp.gravity = m_fGravityModifier;
	ppp.lifetime = m_fStartLifetime._max;
	ppp.size = m_fStartSize._max;
	ppp.speed = m_fStartSpeed._max;

	pc->Initialize(ppp);
}
