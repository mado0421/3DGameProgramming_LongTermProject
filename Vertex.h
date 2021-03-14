#pragma once
class Vertex
{
public:
	XMFLOAT3 m_xmf3Pos;
	XMFLOAT3 m_xmf3Normal;
	XMFLOAT3 m_xmf3Tangent;
	XMFLOAT3 m_xmf3BiTangent;
	XMFLOAT2 m_xmf2UV;

public:
	Vertex() = default;
	Vertex(XMFLOAT3 xmf3Pos, XMFLOAT3 xmf3Normal, XMFLOAT2 xmf2UV) :m_xmf3Pos(xmf3Pos), m_xmf3Normal(xmf3Normal), m_xmf2UV(xmf2UV) {}
	Vertex(XMFLOAT3 xmf3Pos, XMFLOAT3 xmf3Normal, XMFLOAT3 xmf3Tangent, XMFLOAT3 xmf3BiTangent, XMFLOAT2 xmf2UV) 
		:m_xmf3Pos(xmf3Pos), 
		m_xmf3Normal(xmf3Normal),
		m_xmf3Tangent(xmf3Tangent),
		m_xmf3BiTangent(xmf3BiTangent),
		m_xmf2UV(xmf2UV) 
	{}
};

