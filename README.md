# 종합설계
> ### 큰 목표 우선순위
> 1. 빛과 그림자 구현
> 2. 애니메이션 구현
> 3. 포스트 프로세싱 구현(블러 등)
> 4. 충돌검사 및 후처리

# <21.03.14>
## 일정 관리
### 지난 일정
* 21.02.17 - 21.02.21 이전 D3D12 코드 복습
* 21.02.22 - 21.02.28 큐브 메쉬 렌더링 및 D3D12 API 숙지
* 21.03.01 - 21.03.07 디퍼드 쉐이딩을 위한 RenderToTexture와 GBuffer 구현
* 21.03.08 - 21.03.14 디퍼드 쉐이딩 환경에서의 조명 계산

### 이번주 일정
#### 21.03.15 - 21.03.21
* 조명별 쉐도우맵 생성 및 업로드(64개의 쉐도우맵을 올릴 수 있도록)
* SpotLight 그림자 구현
* Cubemap과 GS를 이용한 PointLight 그림자 구현
  - Cubemap Texture 사용법 숙지
  - GeometryShader 사용법 숙지
* Cascaded ShadowMap 을 이용한 Directional Light 그림자 구현
  - Texture Array 사용법 숙지

### 2주 목표
* 그림자 구현 
* 바닥 생성
* 큐브가 아닌 모델을 .obj 포맷으로 불러와서 렌더링

 ## 현재 구현된 사항
- 큐브 메쉬 렌더링(uv, normal, tangent 정보를 가진)
- 디퍼드 쉐이딩을 위한 2 Pass 렌더링
  - Render To Texture
  - DepthBuffer 읽기
  - Texture를 ShaderResource로 사용하여 두 번째 Pass에서 worldPos, normal, color 정보 언패킹하기
  - 언패킹한 정보를 바탕으로 조명 계산하기

### 2021.03.18
<img src="https://user-images.githubusercontent.com/21697638/111553756-58535f80-87c8-11eb-95b5-4fb1c6bab7a7.png" width="70%" height="70%"></img>

# <21.03.21>
## 일정 관리
### 지난 일정
* 21.02.17 - 21.02.21 이전 D3D12 코드 복습
* 21.02.22 - 21.02.28 큐브 메쉬 렌더링 및 D3D12 API 숙지
* 21.03.01 - 21.03.07 디퍼드 쉐이딩을 위한 RenderToTexture와 GBuffer 구현
* 21.03.08 - 21.03.14 디퍼드 쉐이딩 환경에서의 조명 계산
* 21.03.15 - 21.03.21 다중 SpotLight 그림자 구현

### 이번주 일정
#### 21.03.22 - 21.03.28
* Cubemap과 GS를 이용한 PointLight 그림자 구현
  - Cubemap Texture 사용법 숙지
  - GeometryShader 사용법 숙지
* Cascaded ShadowMap 을 이용한 Directional Light 그림자 구현
  - Texture Array 사용법 숙지

### 2주 목표
* 그림자 구현 
* 바닥 생성
* 큐브가 아닌 모델을 .obj 포맷으로 불러와서 렌더링

 ## 현재 구현된 사항
- 큐브 메쉬 렌더링
- 디퍼드 쉐이딩
- 다중 SpotLight 조명 그림자

루트 상수를 한 프레임 내에 여러 번 바꿀 수 있을 것이라 오해하고 있었다. 말 그대로 상수처럼 사용되는 것인데. 그래서 인덱스를 갱신해가며 사용할 수 있을 것이라 믿었던 조명과 쉐도우맵 배열 구조를 전부 부수고 광원별 렌더 패스를 추가하는 식으로 변경하였다. 

### 2021.03.21
<img src="https://user-images.githubusercontent.com/21697638/111884756-f6cd0400-8a06-11eb-90d6-d70c830bd86f.png" width="70%" height="70%"></img>

### 2021.03.22
프로젝트의 단위 1 이 1 m가 되도록 설정하였다. (0.001 = 1mm, 1,000 = 1km)
투영 변환 행렬도 그에 맞게 조절하고 오브젝트와 라이트의 크기도 변경하였다.
이제 메쉬 로드가 된다.
<img src="https://user-images.githubusercontent.com/21697638/111926071-aa5cf380-8aee-11eb-83ce-9b9e3ea0e60d.png" width="70%" height="70%"></img>

###2021.03.25
Point Light Shadow를 위해 TextureManager와 Texture에 CubeMap으로 생성하는 함수를 작성해주고 Header.hlsli에 TextureCube를 추가해주었다.
원래 Texture2DArray로 만들고 그걸 Texture2D로 되지 않을까 하고 올려봤었는데 어림도 없었다.
[Introduction To Textures in Direct3D 11 - Win32 apps | Microsoft Docs](https://docs.microsoft.com/en-us/windows/win32/direct3d11/overviews-direct3d-11-resources-textures-intro)
여기에 보면 큐브맵의 인덱스가 나온다. 저 순서대로 조명마다 뷰 변환 행렬을 만들어줘야 한다.(올릴 땐 투영 변환 행렬 곱하고 전치행렬로 바꿔서)
<img src="https://user-images.githubusercontent.com/21697638/112391901-f2248a80-8d3b-11eb-8d31-36f302a8be15.png" width="70%" height="70%"></img>
