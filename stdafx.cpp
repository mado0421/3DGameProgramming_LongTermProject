#include "stdafx.h"
#include "Texture.h"
#include "Model.h"
#include "Material.h"
#include "Animation.h"
#include "State.h"

TextureManager	g_TextureMng;
ModelManager	g_ModelMng;
MaterialManager g_MaterialMng;
AnimationManager g_AnimMng;
int gTestInt = 2;
AnimationUploader* g_AnimUploader;

UINT gnCbvSrvDescriptorIncrementSize = 0;