#include "stdafx.h"
#include "Texture.h"
#include "Model.h"
#include "Material.h"
#include "Animation.h"
#include "State.h"

TextureManager	gTextureMng;
ModelManager	gModelMng;
MaterialManager gMaterialMng;
AnimationManager gAnimMng;
int gTestInt = 0;
AnimationController* g_AnimCtrl;

UINT gnCbvSrvDescriptorIncrementSize = 0;