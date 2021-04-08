#include "stdafx.h"
#include "Texture.h"
#include "Model.h"
#include "Material.h"
#include "Animation.h"

TextureManager	gTextureMng;
ModelManager	gModelMng;
MaterialManager gMaterialMng;
AnimationManager gAnimMng;

UINT gnCbvSrvDescriptorIncrementSize = 0;