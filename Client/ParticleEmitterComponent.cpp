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
	, m_fStartSize(fRange(0.1f, 0.5f))
	, m_xmf3StartRotation(XMFLOAT3(0,0,1))
	, m_fRotationAngle(30)
	, m_fFlipRotation(0)
	, m_fGravityModifier(0.1f)
	, m_RateOverTime(1)
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
	// ���⼭ erase() �Ѵٰ� �ؼ� ���� �ּҰ� �������ų� ���� �����Ƿ� �Ƚ��ϰ� erase() �ص� �ȴ�.
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
	// m_strMaterialName�� ""�� ���� ���� ��~ ������.
	// meshRenderer�� �ٸ��� particle�� �������� render()�� ���߰� �־�� �Ѵ�.
	// Particle�� material�� ���� �ʴ´�. material Set�� ���⼭ �� �� �ϰ� �� �ڷ� render()�� ��.
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
		// ���� �� ��~
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
