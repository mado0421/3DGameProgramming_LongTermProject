#pragma once
#include "Component.h"

struct fRange {
	float _min, _max;

	fRange(float min, float max) :_min(min), _max(max) {}
};

enum class ParticleStopAction {
	None, Disable, Destroy
};

struct ParticleBurstInfo {
	float time;
	int count;
	int cycles;	//0:infinit
	float interval;
	float probability; //0~1
};

struct ParticlePropertiesPack {
	float lifetime;
	XMFLOAT3 currPos;
	XMFLOAT3 direction;
	float speed;
	float size;
	float gravity;
};

class ParticleEmitterComponent : public Component
{
public:
	ParticleEmitterComponent() = delete;
	ParticleEmitterComponent(Object* pObject, vector<Object*>* particlePool);
	~ParticleEmitterComponent();

public:
	virtual void Update(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);

public:
	void SetMaterialByName(const char* strMaterialName);

private:
	void AddParticle();

	Object* GetUsableParticle();
	void InitializeParticle(Object* pObject);

private:
	float		m_fDuration;
	bool		m_bLooping;
	bool		m_bPrewarm;
	float		m_fStartDelay;
	fRange		m_fStartLifetime;
	fRange		m_fStartSpeed;
	fRange		m_fStartSize;
	XMFLOAT3	m_xmf3StartRotation;
	float		m_fRotationAngle;
	float		m_fFlipRotation;
	float		m_fGravityModifier;
	int			m_RateOverTime;
	ParticleStopAction			m_StopAction;
	vector<ParticleBurstInfo>	m_vecEmissionBurst;

	float		m_fCreateCooltime;
	float		m_fCurrCooltime;

protected:
	vector<Object*> m_vecParticle;
	string			m_strMaterialName;

	vector<Object*>* m_pSceneParticlePool;
};

