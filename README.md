
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


### 이번주 일정
#### 21.03.29 - 21.04.04
* Cascaded ShadowMap 을 이용한 Directional Light 그림자 구현
* Object 렌더링 시에 텍스처 등을 어떻게 저장할 것인지 정리(자료구조 측면에서)
* 마테리얼 구조체 업로드 준비
* FBX SDK로 파일 읽기(애니메이션 구현 준비)

### 2주 목표
* Object 자료구조 정리(또는 Component 인터페이스 작성)
* FBX SDK 다운로드 및 사용법 정리

 ## 현재 구현된 사항
- 큐브 메쉬 렌더링
- 디퍼드 쉐이딩
- 다중 SpotLight 조명 그림자
- 다중 PointLight 조명 그림자
- Cascade 를 TextureArray로 DepthBuffer에 그리는 것까지 진행

### 2021.03.29
원래 3월 28일까지 CSM을 만들었어야 했는데 26일에 깊이버퍼에 그려지는 것까지만 확인하고 자잘한 문제가 있던 것을 28일에 해야지 하고 내버려뒀다.
그리고 28일에 심하게 체해서 하루종일 정말 아무것도 못 했다.
이 문서도 어제 정리했어야 했는데.. 하여간 그림자 정말 얼마 안 남았으니 빨리 쳐내고 애니메이션을 하자.
블렌더도 공부해보고 해야 할 것이 많다.

### 2021.03.31
28, 29, 30일을 내리 쉬었고 다시 작업 시작했다.
CSM 구현 시에 많은 문제가 있었는데 현재 진행 상황은 다음과 같다.
- 뷰 프러스텀의 꼭짓점 좌표를 구하기 위해 CameraView 변환 행렬의 역행렬을 곱해 WorldSpace로 가져오고, LightSpace로 변환한 뒤에, 각 캐스케이드 별로 min/max XYZ를 구하여 직교 투영 행렬을 만들 때 필요한 width와 height, Znear, Zfar을 구했다.
- 각 캐스케이드 별로 위치를 구하여 View 변환 행렬을 만들어주고 위에서 구한 투영행렬과 곱해서 GPU에 올려줬다.
- 그래서 일단 되긴 한다...
- 다만, 카메라 회전 시에 쉐도우 맵 텍스처 안에 안 들어가는건지.. 아니면 캐스케이드 인덱스를 구하는 식에 문제가 있는건지 중간에 의도하지 않은 그림자가 생기는 문제가 있다

<img src="https://user-images.githubusercontent.com/21697638/113150035-9cd20700-926e-11eb-9230-03f8149263ce.png" width="70%" height="70%"></img>
<img src="https://user-images.githubusercontent.com/21697638/113150117-b4a98b00-926e-11eb-96b3-3f682aa57016.png" width="70%" height="70%"></img>