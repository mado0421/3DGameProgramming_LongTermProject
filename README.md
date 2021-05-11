
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

### 이번주 일정
#### 21.04.12 - 21.04.18
* Animation Parser, Animation Controller 구현

진척사항이 없어서 커밋하지 않았다.

### 이번주 일정
#### 21.04.19 - 21.04.25
* Animation Parser, Animation Controller 구현

진척사항이 없어서 커밋하지 않았다.

### 이번주 일정
#### 21.04.26 - 21.05.02
* 휴식

### 이번주 일정
#### 21.05.03 - 21.05.09
* Animation Parser, Animation Controller 구현

### 2021.05.03
Fbx Parser 구현 성공. 추후 내용 정리해서 올릴 것.

### 2021.05.04
본 프로젝트에 적용 완료. 코드 정리해서 올릴 것.

<img src="https://user-images.githubusercontent.com/21697638/116945821-8840be80-acb3-11eb-8188-742572a047b6.png" width="70%" height="70%"></img>


- Parser에서 VertexNormal이 제대로 입력되지 않아 Split per-vertex Normals 옵션을 켤 수 없었는데 해당 부분을 수정.
- PSO를 변경하고 추가하여 애니메이션이 적용된 오브젝트도 Shadow Occluder로 작용.

<img src="https://user-images.githubusercontent.com/21697638/116962876-3fecc500-ace2-11eb-8b04-df5c9c8aa109.png" width="70%" height="70%"></img>

### 2021.05.06
미리 만들어둔 사람 모델을 리깅하여 사용하였다.

Parser에서 Mesh를 저장할 때 하나의 Vertex에 영향을 주는 Bone이 5개 이상인 경우가 나오는 문제가 있었다.
하나의 Vertex에 영향을 주는 Bone이 4개를 초과하는 경우, 가장 작은 weight값을 가진 Bone을 지우고 해당 weight값을 나눠 다른 Bone들의 weight에 더해주는 방법으로 수정하였다.

<img src="https://user-images.githubusercontent.com/21697638/117238481-edd5ac00-ae67-11eb-9259-b6ca4016ee7f.gif" width="70%" height="70%"></img>

이전에 구현해둔 쿼터니언 보간 방식은 각 요소별로 CatmullRom 보간을 하는 방법이었기 때문에, 회전값이 튀는 문제가 있었다.
같은 회전값을 나타내더라도 표현하는 쿼터니언 값이 다를 수 있던 문제였는데(아마?) 이를 요소별로 단순하게 보간하여 발생한 것 같아 XMQuaternionSlerp()을 사용하여 보간하게 변경하였다.

FBX Asset Import 시에
1. position과 normal의 Y와 Z를 Swap한다.
2. position과 normal의 Z에 -1을 곱한다.

VertexShader에서
1. position과 normal 변환결과값의 X에 -1을 곱한다.
(tangent나 uv에 비슷한 처리를 해야 하는지는 아직 모름)

<img src="https://user-images.githubusercontent.com/21697638/117239997-d3e99880-ae6a-11eb-99d1-6542dd5d441b.gif" width="70%" height="70%"></img>

### 2021.05.07

AnimationController를 상속받는 HumanoidAnimCtrl를 만들고 HumanoidObject가 갖고 있는 HumanoidState에 따라 다른 Clip을 재생하도록 하였다.

<img src="https://user-images.githubusercontent.com/21697638/117376845-f2a06b80-af0c-11eb-8174-a37296c1df5d.gif" width="70%" height="70%"></img>

AnimationController에서는 입력받은 State와 Time으로 행렬을 만들어 Set까지 하고 있다.
이것으로 컨트롤러에서 할 일은 일단 끝. (블렌딩 등을 제외하고)
AnimatedObject(특히 HumanoidObject를 기준으로)에서 State 전환을 만들어야 한다.
외부에서 Object에게 MoveForward() 명령어를 주었을 때, 만약 해당 Object가 직전에 Jump를 하여 InAir State라면, MoveForward()를 호출해도 아무런 효과가 없어야 한다.

### 이번주 일정
#### 21.05.10 - 21.05.17
* State와 Transaction 구현
* Object의 상황에 따라 State가 전환
* 소스코드 정리

### 2주 목표
* UI와 Font 구현(한글까지 지원할 수 있도록)

### 2021.05.10
State 클래스를 다음과 같이 작성하고 Humanoid_Idle, Humanoid_Walk 클래스를 State 클래스를 상속받아 구현하였다.

	class State {
	public:
		State(const char* strName, Object* obj) :m_strStateName(strName), m_pObject(obj) {}

		void AddTransation(Transaction* transation) { 	m_vecTransactions.push_back(transation); }
		bool IsSatisfyTransaction(const char* strStateName);
		void ChangeStateTo(const char* strStateName);

		virtual void EnterState() {}
		virtual void LeaveState() {}
	public:
		virtual void MoveForward() = 0;
		virtual void Update(float fTimeElapsed) = 0;

	public:
		string m_strStateName;

	protected:
		vector<Transaction*>	m_vecTransactions;
		Object*					m_pObject;
	};

Idle State인 플레이어 캐릭터에게 WalkForward() 명령을 주면 Walk State로 전환할 수 있는 Transaction이 있는지 검사하고, 있으면 해당 Transaction의 조건에 적합한지 확인 후에 맞으면 해당 State로 전환한다.

	if(IsSatisfyTransaction("strStateName")) ChangeStateTo("strStateName");

### 2021.05.11
이번 2주 목표를 'UI와 Font 구현(한글까지 지원할 수 있도록)'에서 '손에 무기 들려주기, IK 구현, 애니메이션 블렌더(이건 시도까지만)'으로 변경
Bump 매핑을 구현하는 것을 먼저 한다.

https://www.textures.com/ 에서 albedo, normal, roughness맵을 다 갖고 있는 Texture Set를 받아 테스트에 사용하였다.
사용한 Set은 [Medieval Cobblestone Pavement - PBR0024 (textures.com)](https://www.textures.com/download/PBR0052/133087)

Specular Map과 Roughness Map이 갖는 의미의 차이점을 잘 모르겠기에 지금은 Specular Map의 슬롯을 기본으로 비워두고, Albedo Map(이것도 Diffuse Map과의 차이점을 모르겠다)의 Alpha 채널에 Roughness Map의 정보를 넣어 하나로 합친 뒤에 사용하였다.
즉, 지금 마테리얼이 Texture Set으로 갖는 정보는 Diffuse.rgb, Roughness.r(==Diffuse.a에 들어감), Normal.rgb 이렇게 있다.
재질의 프레넬 값을 가지는 텍스처는 아직 보지 못했기에 따로 재질에 따라 제작할 필요가 있다. 또는 마테리얼마다 지정해주거나.
다만, 지금 사용하고 있는 마테리얼마다 지정해주는 방식은 한 마테리얼에서 다양한 재질을 표현할 수 없다는 문제가 있다.
예를 들어, 하나의 모델에서 한 부분은 천 재질이고, 다른 부분은 가죽 재질일 때, Roughness 값은 다르게 줄 수 있지만 프레넬 값은 동일하게 줄 수 밖에 없다. 금속의 경우, 프레넬 값이 매우 큰 것이 다른 재질과의 차이점인데 하나의 마테리얼에서 그것을 표현할 수 없다는 것이 문제가 된다.
하지만 이와 별개로 마테리얼이라는 표현 자체가 재질을 의미하는 만큼, 마테리얼 하나에 재질 하나만을 표현해야 하는 것이 아닌가 같은 것도 고려해봐야 할 것 같다.
일단 지금은 프레넬 값을 마테리얼마다 고정하여 주고 있다.

<img src="https://user-images.githubusercontent.com/21697638/117751361-edfeee80-b24f-11eb-9d7d-5062e195720e.png" width="70%" height="70%"></img>

<img src="https://user-images.githubusercontent.com/21697638/117751397-fe16ce00-b24f-11eb-9328-6e5f1cd4879c.png" width="70%" height="70%"></img>

아래 사진은 하드코딩 되어 있는 프레넬 값에 차이를 줘 렌더링 한 결과물이다.
[왼쪽 - 0.1, 오른쪽 - 0.9]

SpotLight 생성 시에 SpotLight의 각도를 SpotPower로 조절해주고 있었으나, LightDirection이 normalize 되지 않은 채로 들어가 의도하지 않은 결과가 나오는 점을 수정.

<img src="https://user-images.githubusercontent.com/21697638/117754539-7b910d00-b255-11eb-9875-7b5450ee4364.png" width="70%" height="70%"></img>

3ds Max에서 지금 필요한 모든 정보들을 얻기 위해선 다음과 같은 처리가 필요하다.
1. UnwrapUVW를 하여 UV를 생성
2. TurnToPoly를 하여 모든 Polygon Size를 3 이하로 줄여, 수동 Triangulate (Export 옵션에서 선택하는 Triangulate 만으로는 Binormal과 Tangent가 제대로 생성되지 않았다)
3. Split per-Vertex normals, Tangents and Binormals, Triangulate 에 체크
4. Units은 Automatic, Axis는 Y-up
5. Import 할 때, Position, Normal, Tangent는 YZ Swap, Z 반전
6. Import 할 때, UV는 V 반전

<img src="https://user-images.githubusercontent.com/21697638/117795033-c1b39400-b288-11eb-8eba-6b0d1f3a59c3.png" width="70%" height="70%"></img>

다른 오브젝트를 Humanoid Object의 R Hand Bone까지 옮기는 것을 구현했다.
여기서 오브젝트가 YZ평면으로 반전된 곳에 위치하는 문제가 있었는데
이것은 AnimClip을 import 할 때, 해당 평면에 대해 반전되서 들어오는 것을 별다른 처리 없이 그대로 사용한 뒤,
VS에서 모든 Vertex에 대해 일괄적으로 position.x *= -1를 해주는 것으로 마무리했기 때문이다. (이렇게 해야 의도한 대로 좌우가 맞게 출력되므로)
지금은 옮기려는 오브젝트에 평면대칭행렬을 곱해서 해결했지만 위에서 말한 AnimClip 행렬들에 평면대칭행렬을 곱해서 VS에서 추가적인 계산을 할 필요 없도록 수정해야 할 것