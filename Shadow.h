#pragma once
/*
* 쉐도우맵 사용 전략.
* 
* 목적: 그림자를 드리울 조명은 쉐도우맵이 있어야 한다.(dir: 3개, point: 6개, spot: 1개)
* 문제: 근데 렌더 패스 한 번에 조명 계산은 최대 64번 해야 하고 쉐이더 리소스는 하나밖에 못 올린다.
* 이론: 쉐도우맵을 Texture2DArray로 만들어서 64개를 한 번에 올릴 수 있지 않을까?
* 
* 
* 
* 
*/
class DepthTexture;

constexpr int shadowMapWidth = 256;

class Shadow
{
public:
	void Init(ID3D12Device* pd3dDevice, D3D12_CPU_DESCRIPTOR_HANDLE& cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle);
	void RenderShadow(ID3D12GraphicsCommandList* pd3dCommandList, UINT shadowIdx) {
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

		pd3dCommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
		pd3dCommandList->OMSetRenderTargets(0, nullptr, TRUE, &dsvHandle);
	}

private:
	UINT					m_nMaxShadowMap = 64;
	DepthTexture*			m_pDepthStencilTexture;
	ID3D12DescriptorHeap*	m_pd3dDsvDescriptorHeap;
};

