#pragma once
#define Interface struct
Interface IComponent {
	IComponent() = default;
	virtual void Update(const float fElapsedTime) = 0;
};

class ITransform : public IComponent {
public:
	ITransform();
	
	void Move();
	void Rotate();
private:
	XMFLOAT4X4 m_xmf4x4world;
};

