
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

### 2021.04.01
CSM을 구현했다. 구현하면서 있었던 문제들과 해결법은 다음과 같다.
1. 깊이텍스처배열에 렌더는 되는데 카메라가 이동해도 깊이텍스처의 내용은 바뀌지 않음 -> 조명의 위치를 각 캐스케이드의 시작점으로 이동시켜 해결
2. 거리에 따라 잘못된 캐스케이드가 선택되어 의도하지 않은 그림자가 발생 -> 월드 공간에서 픽셀의 위치와 카메라 간의 거리로 캐스케이드를 선택하게 하지 않고, 월드 공간 픽셀의 위치를 카메라 공간으로 옮긴 뒤에 z값으로 캐스케이드를 선택하게 변경(원래 캐스케이드의 바운더리 박스를 만들 때 쓰는 뷰 프러스텀도 카메라 뷰 공간에 있었으니까)
3. 깊이텍스처의 일부(오른쪽과 아래쪽) 공간에 렌더링이 되지 않음 -> 이건 다른 쉐도우 맵을 생성할 때도 있었으나 평상시에는 눈에 띄지 않아서 몰랐던 문제로서 쉐도우 맵을 렌더링 할 때, 뷰 포트의 크기만 쉐도우 맵의 크기에 맞게 변경해주고 시저렉트는 변경해주지 않아서 생긴 문제였음(쉐도우 텍스처의 크기를 2048로 바꾸면 1/4만 그려지는 문제가 있었는데 아... 암튼 해결함)

이렇게 적어보면 별 문제 없어보이지만 마땅한 디버깅 방법을 알지 못했던 터라 주먹구구식으로 시도한 일이 많았다. 효율적인 디버깅 방법이 절실함..

이제 오브젝트 자료구조와 마테리얼 등을 정리하고 구현한 뒤에 애니메이션으로 넘어갈 준비를 하면 될 것 같다.

<img src="https://user-images.githubusercontent.com/21697638/113239609-0b05e080-92e6-11eb-8cce-9dda85a663b9.png" width="70%" height="70%"></img>
<img src="https://user-images.githubusercontent.com/21697638/113239658-253fbe80-92e6-11eb-99b4-921ac5e872fa.png" width="70%" height="70%"></img>

### 2021.04.04
이번 주 내내 어딘가 아프거나 고장이 나서 남은 주말까지 포함해서 한 주를 쉬어가는 걸로 결정했다.
나는 더 이상 3ds Max를 사용할 수 없고, 내가 원하는 조건(diffuseMap, normalMap, specularMap이 있고... 아마 앞으로 애니메이션까지 들어가면 더 까다로워질)으로 완벽하게 준비된 에셋을 구하기도 어려울 것 같아서 Blender 2.92로 에셋을 만들면서 쉬었다. 이틀 쓰면 간단한 캐릭터 하나 정도는 만들 수 있을거라 생각했는데 오랜만에 모델링을 하기도 했고, 블렌더가 맥스와 완전히 사용방법이 달라서 적응하는 것도 엄청 오래 걸렸다. 하지만 편하게 쉬었으니 괜찮다고 생각. 앞으로 쉴 때 모델링도 겸해서 해보는 것으로.

<img src="https://user-images.githubusercontent.com/21697638/113578416-c23f8600-965d-11eb-8275-8df5d09a4e3a.png" width="70%" height="70%"></img>

### 이번주 일정
#### 21.04.05 - 21.04.11
* Object 렌더링 시에 텍스처 등을 어떻게 저장할 것인지 정리(자료구조 측면에서)
* 마테리얼 구조체 업로드 준비
* FBX SDK로 파일 읽기(애니메이션 구현 준비)

### 2주 목표
* Object 자료구조 정리(또는 Component 인터페이스 작성)
* FBX SDK 다운로드 및 사용법 정리

### 2021.04.05
자원 관리 자료구조를 작성했다.
원래 Scene에 속해있던 TextureMng와 Object에 속해있던 vector<mesh*>의 자원들을 전역으로 올리고 한 번씩만 생성하게 변경하였다.(원래는 같은 mesh여도 여러 Object일 경우, 여러 번 생성하여 메모리 낭비가 있었다)
따라서 지금 구조는 다음과 같다.

> 전역에 gTextureMng, gModelMng, gMaterialMng 로 자원관리
> 처음에 AssetData를 읽어서 이 Scene에서 어떤 Asset(obj, dds 등)을 사용하는지 확인
> 위에서 확인한 AssetList를 바탕으로 Model(mesh들의 집합)과 Texture를 Load
> MaterialData를 읽어서 Material을 생성
> (defaultMaterial도 생성하여 mat이 없는 object의 경우, defaultMaterial을 사용하게 함)
> Object에서 Render를 할 때, Material과 Model name을 가지고 gMaterialMng, gModelMng에서 Set과 Render하게 변경
> (이전에 사용한 Material과 같은 경우, 추가로 Set을 하지 않게 이전에 사용한 mat의 이름을 Mng에서 저장)

자원 관리를 위해 Data 파일만 3개를 사용하게 됨
자원 사용을 위해선 다음의 순서를 거쳐야 함
1. 먼저 Data 폴더의 AssetList에 사용할 에셋을 추가한다
2. 해당 에셋을 Assets 폴더에 추가한다
3. MaterialData에 추가할 Texture를 더해준다
4. ObjectData에 Material과 Model을 더해준다

하여간 해야 할 것이 많다

추가로 SpecularMap을 추가하였다
스펙큘러맵을 이렇게 사용하는게 맞는진 모르겠으나 일단 0~1 사이의 값을 가지고 roughness 값으로 사용된다. 이게 있으면 한 Texture를 사용하는 Model에서 다양한 roughness 값을 줄 수 있게 된다
지금 light를 계산하는 hlsl에서 roughness * 256 해서 사용하고 있으므로 기억해두자
Fresnel값도 어떻게 주고 싶은데 그것까지는 아직 못해주고 있다
추가로 RGB Texture를 써야 할 지 고민해봐야 할 것 같음

<img src="https://user-images.githubusercontent.com/21697638/113580174-45fa7200-9660-11eb-841f-0f87b0694b88.png" width="70%" height="70%"></img>

### 2021.04.08
Blender에서 리깅과 애니메이션 작업까지 한 모델을 FBX로 Export하여 사용하기 위해 작업했다.
스키닝 애니메이션은 한 차례 구현해 본 적이 있어 이론을 복습하는 것은 쉬웠다.
FBX SDK의 사용법이 어렵고 복잡해서 많이 힘들었는데 Parser를 작성한 지금도 현재의 방식이 그리 맘에 들지 않는다.
블렌더에서 키프레임을 Export하고 다시 Import 해보니 Max에서 리샘플링 옵션을 켠 것처럼 모든 프레임에 키프레임이 생기는걸 볼 수 있었다. 특정 키프레임의 시간만 얻는 것을 포기하고 모든 프레임에서 toWorld 행렬을 뽑아냈다.
빠르게 로드하기 위해 바이너리 포맷으로 만든 뒤에 한 번에 데이터 전체를 읽어내는 식으로 만들었다. (이후 처리 공정이 있긴 하지만)
지금은 루트 상수버퍼를 추가하고 애니메이션 본 변환 행렬을 올리는 것까지 진행했다.
앞으로 해야 할 일은 다음과 같다.

1. 변환 행렬을 시간에 따라 보간.
2. 이 오브젝트가 FBX 모델인지 OBJ 모델인지, 애니메이션이 적용되어있는 메쉬인지 등을 구분.
3. 각자 다른 PSO를 사용하여 렌더링.

블렌더에서 FBX로 Export 할 때, 토폴로지를 D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP이 아니라 D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST로 뽑고 싶은데 Export 옵션에선 못 찾겠다. 
pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); 
이 함수를 찾아서 수정해야 할 것. (FBX모델은 TRIANGLESTRIP로 변환했으니까)

### 이번주 일정
#### 21.04.12 - 21.04.18
* AnimationClip, AnimationCtrl 등 Animation 자료구조 구현

### 2주 목표
* Animation 구현

### 2021.04.12
Blender에서 Export한 FBX 파일에 문제가 많아서 Max로 작업환경을 변경했다. Blender에서 있었던 문제는 다음과 같다.

- FBX 포맷으로 Export 할 경우, Vertex가 없어지는 문제가 있다. 정확히는 프리미티브 토폴로지가 트라이앵글도 아니고 트라이앵글 스트립도 아니고 전체 Face의 절반이 사라지는 문제가 있다... Export 한 결과물을 다시 Import 했을 때 Mesh에 문제가 있던 것 같아 보이진 않았는데 대체 어떻게 저장을 하고 있는 것인지 모르겠음.
- Export 할 때, 3ds Max에서의 '리샘플링 올' 옵션이 자동으로 적용되는 듯하다... Keyframe의 Time 값을 얻기 위해 Parser를 돌렸을 때, 모든 프레임의 시간이 나오길래 뭔가 싶어서 다시 Import 해봤는데 모든 프레임에 키프레임이 찍혀있는걸 보고 3ds Max를 설치했다.

<img src="https://user-images.githubusercontent.com/21697638/114394625-5ec1d500-9bd6-11eb-8125-32ac869a7680.png" width="70%" height="70%"></img>

Blender에서 Export 하는 것은 문제가 너무 많아서 다시 Max에서 작업하는 것으로 변경했다. 그로 인해 AnimationClip의 자료구조가 변경되었고(이젠 키프레임을 리샘플링 올을 하지 않은 결과로 처리할 수 있으니까) Parser의 구조도 변경하였다.

AnimationClip의 구조는 다음과 같다.

    struct Keyframe {
	    float keytime;
	    float4 rotation;
	    float3 translation;
	}
	
	using Bone = vector<Keyframe>;
	
	AnimationClip DataStructure
	
	nBone
	eachBone: nKeyframe
	sizeof(Keyframe) * nTotalKeyframe
	
   Parser를 구현했고 이에 맞춰서 본 프로젝트에서 Import 할 수 있게끔 변경했다.  FBX로 읽은 Mesh를 렌더링 하는 것까지 구현했다. AnimationClip을 읽어오는 것까지 성공했다.

### 2021.04.13
AnimationClip에서 읽은 FrameData를 GPU에 올려 VS에서 사용할 수 있게 하였다. 
VS Input Semantic으로 TEXCOORD는 되고 TEXCOORD1은 안된다.
FrameData가 문제 없이 올라가는 것을 확인했고 이전 Frame과 다음 Frame 간의 보간을 구현하였다.
3ds Max에서 회전 값은 TCB, 위치 값은 베지어 방식으로 보간한다고 하여 찾아보았는데,
TCB는 Tension, Continuity, Bias의 줄임말로 CatmullRom Spline에 bias 값을 넣은 정도로 보였다.
나는 T, C, B 모두 Default 값으로 두고 에셋을 만들 예정이므로 CatmullRom 보간만 할 수 있으면 된다.
그리고 그건 XMVectorCatmullRom() 라는 함수로 이미 구현되어 있다.

다만, CatmullRom 스플라인은 보간하려는 두 값의 연속성? 미분값? 속도?값 을 정하기 위해 그 앞과 뒤의 값도 필요로 하기 때문에 값을 4개 전달해야 한다.
이것과 지금 AnimationClip의 저장방식(frameLength와 같은 keytime을 가지는 맨 마지막 키프레임이 있을 수도, 없을 수도 있는 구조)에 따른 루프 애니메이션과 루프가 아닌 애니메이션의 키프레임 인덱스를 정하는 방법을 전부 구현하지는 않았다.
지금 구현한 방법은 맨 앞과 맨 마지막이 이어지지 않는다는 가정 하에 구현하였다.

현재 프레임 인덱스를 구하는 것 자체는 크게 문제가 없는 것 같고 변환 행렬 값에 문제가 있어서 제대로 애니메이트 되지 않고 있다.
