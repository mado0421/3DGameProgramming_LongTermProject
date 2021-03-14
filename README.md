# 종합설계
## 일정 관리
> ### 지난 일정
> * 21.02.17 - 21.02.21 이전 D3D12 코드 복습
> * 21.02.22 - 21.02.28 큐브 메쉬 렌더링 및 D3D12 API 숙지
> * 21.03.01 - 21.03.07 디퍼드 쉐이딩을 위한 RenderToTexture와 GBuffer 구현
> * 21.03.08 - 21.03.14 디퍼드 쉐이딩 환경에서의 조명 계산

> ### 이번주 일정
>  #### 21.03.15 - 21.03.21
> * 조명별 쉐도우맵 생성 및 업로드(64개의 쉐도우맵을 올릴 수 있도록)
> * SpotLight 그림자 구현
> * Cubemap과 GS를 이용한 PointLight 그림자 구현
>   - Cubemap Texture 사용법 숙지
>   - GeometryShader 사용법 숙지
> * Cascaded ShadowMap 을 이용한 Directional Light 그림자 구현
>   - Texture Array 사용법 숙지

> ### 2주 목표
> * 그림자 구현
> * 바닥 생성
> * 큐브가 아닌 모델을 .obj 포맷으로 불러와서 렌더링

> ### 큰 목표 우선순위
> 1. 빛과 그림자 구현
> 2. 애니메이션 구현
> 3. 포스트 프로세싱 구현(블러 등)
> 4. 충돌검사 및 후처리
> 
 
 ## 현재 구현된 사항
- 큐브 메쉬 렌더링(uv, normal, tangent 정보를 가진)
- 디퍼드 쉐이딩을 위한 2 Pass 렌더링
  - Render To Texture
  - DepthBuffer 읽기
  - Texture를 ShaderResource로 사용하여 두 번째 Pass에서 worldPos, normal, color 정보 언패킹하기
  - 언패킹한 정보를 바탕으로 조명 계산하기

## 코드 작성 규칙
```c++
// 클래스 작성 규칙
// 함수를 먼저 작성한다.
// public, protected, private 순으로 작성한다.
class Test {
public:
	Test();
	~Test();
public:
	virtual void Update();
private:
	void SomethingSecret();
public:
	float 			m_fTime;
	string 			m_strName;
protected:
	XMFLOAT4X4 		m_xmf4x4World;
	Other* 			m_pOther;
	Other** 			m_ppOther;
	vector<Other> 		m_vecOther;
	unordered_map<Other> 	m_uomOther;		
}

class Object {
public:
	Object();
	Object(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Rotation)
		:m_xmf3Position(xmf3Position)
		,m_xmf3Rotation(xmf3Rotation);
	~Object();
public:
	virtual void Update(float fElapsedTime);
protected:
	void CreateCBV();
private:
	XMFLOAT3 m_xmf3Position;
	XMFLOAT3 m_xmf3Rotation;
}

// 정보를 읽어와서 생성해야 할 때, 
// 자료구조를 반환한 뒤에 해당 자료를 바탕으로 생성할 것.
void Scene::Initialize() {
	// Scene->m_vecObj 으로 관리중일 때.
	// vector<OBJECT_DESC> Load(const char* filePath);
	ObjectDataImporter objDataImporter;
	vector<OBJECT_DESC> vecObjDesc = objDataImporter.Load("Data/ObjectData.txt");
	
	for (int i = 0; i < vecObjDesc.size(); i++) {
		Object* tempObj = new Object(m_pd3dDevice, m_pd3dCommandList, m_d3dCbvCPUDescriptorStartHandle, m_d3dCbvGPUDescriptorStartHandle);
		tempObj->Move(vecObjDesc[i].position);
		m_vecObject.push_back(tempObj);
	}
}

```

## 리드미 수정 이력
2021.03.14 README.md 생성 및 코드 작성 규칙 작성