#pragma once

struct AffectedBoneInfo {
	int stencil;
	float weight;
	AffectedBoneInfo()
		: stencil(0)
		, weight(0.0f)
	{}
	AffectedBoneInfo(int stc, float weight)
		: stencil(stc)
		, weight(weight)
	{}
};

class BoneMask {
public:
	enum class PreDefined {
		eUpperBody = 0,
		eLowerBody = 1
	};
	BoneMask(PreDefined option, const int stencil) {
		switch (option)
		{
		case BoneMask::PreDefined::eUpperBody:

			affectedBoneInfo[2]  = AffectedBoneInfo( stencil, 0.1f );	// Spine
			affectedBoneInfo[3]  = AffectedBoneInfo( stencil, 0.4f );	// Spine1
			affectedBoneInfo[4]  = AffectedBoneInfo( stencil, 0.9f );	// Spine2
			affectedBoneInfo[5]  = AffectedBoneInfo( stencil, 1.0f );	// Neck
			affectedBoneInfo[6]  = AffectedBoneInfo( stencil, 1.0f );	// L Clavicle
			affectedBoneInfo[7]  = AffectedBoneInfo( stencil, 1.0f );	// L UpperArm
			affectedBoneInfo[8]  = AffectedBoneInfo( stencil, 1.0f );	// L Forearm
			affectedBoneInfo[9]  = AffectedBoneInfo( stencil, 1.0f );	// L Hand
			affectedBoneInfo[10] = AffectedBoneInfo( stencil, 1.0f );	// L Finger0
			affectedBoneInfo[11] = AffectedBoneInfo( stencil, 1.0f );	// L Finger01
			affectedBoneInfo[13] = AffectedBoneInfo( stencil, 1.0f );	// L Finger1
			affectedBoneInfo[14] = AffectedBoneInfo( stencil, 1.0f );	// L Finger11
			affectedBoneInfo[16] = AffectedBoneInfo( stencil, 1.0f );	// L Finger2
			affectedBoneInfo[17] = AffectedBoneInfo( stencil, 1.0f );	// L Finger21
			affectedBoneInfo[19] = AffectedBoneInfo( stencil, 1.0f );	// L Finger3
			affectedBoneInfo[20] = AffectedBoneInfo( stencil, 1.0f );	// L Finger31
			affectedBoneInfo[22] = AffectedBoneInfo( stencil, 1.0f );	// L Finger4
			affectedBoneInfo[23] = AffectedBoneInfo( stencil, 1.0f );	// L Finger41
			affectedBoneInfo[25] = AffectedBoneInfo( stencil, 1.0f );	// R Clavicle
			affectedBoneInfo[26] = AffectedBoneInfo( stencil, 1.0f );	// R UpperArm
			affectedBoneInfo[27] = AffectedBoneInfo( stencil, 1.0f );	// R Forearm
			affectedBoneInfo[28] = AffectedBoneInfo( stencil, 1.0f );	// R Hand
			affectedBoneInfo[29] = AffectedBoneInfo( stencil, 1.0f );	// R Finger0
			affectedBoneInfo[30] = AffectedBoneInfo( stencil, 1.0f );	// R Finger01
			affectedBoneInfo[32] = AffectedBoneInfo( stencil, 1.0f );	// R Finger1
			affectedBoneInfo[33] = AffectedBoneInfo( stencil, 1.0f );	// R Finger11
			affectedBoneInfo[35] = AffectedBoneInfo( stencil, 1.0f );	// R Finger2
			affectedBoneInfo[36] = AffectedBoneInfo( stencil, 1.0f );	// R Finger21
			affectedBoneInfo[38] = AffectedBoneInfo( stencil, 1.0f );	// R Finger3
			affectedBoneInfo[39] = AffectedBoneInfo( stencil, 1.0f );	// R Finger31
			affectedBoneInfo[41] = AffectedBoneInfo( stencil, 1.0f );	// R Finger4
			affectedBoneInfo[42] = AffectedBoneInfo( stencil, 1.0f );	// R Finger41
			affectedBoneInfo[44] = AffectedBoneInfo( stencil, 1.0f );	// Head

			break;
		case BoneMask::PreDefined::eLowerBody:

			affectedBoneInfo[1]	 = AffectedBoneInfo( stencil, 0.9f );	// Pelvis
			affectedBoneInfo[2]	 = AffectedBoneInfo( stencil, 0.4f );	// Spine
			affectedBoneInfo[3]	 = AffectedBoneInfo( stencil, 0.1f );	// Spine1
			affectedBoneInfo[46] = AffectedBoneInfo( stencil, 1.0f );	// L Thigh
			affectedBoneInfo[47] = AffectedBoneInfo( stencil, 1.0f );	// L Calf
			affectedBoneInfo[48] = AffectedBoneInfo( stencil, 1.0f );	// L Foot
			affectedBoneInfo[49] = AffectedBoneInfo( stencil, 1.0f );	// L Toe0
			affectedBoneInfo[51] = AffectedBoneInfo( stencil, 1.0f );	// R Thigh
			affectedBoneInfo[52] = AffectedBoneInfo( stencil, 1.0f );	// R Calf
			affectedBoneInfo[53] = AffectedBoneInfo( stencil, 1.0f );	// R Foot
			affectedBoneInfo[54] = AffectedBoneInfo( stencil, 1.0f );	// R Toe0
			break;
		default:
			break;
		}
	}

	AffectedBoneInfo affectedBoneInfo[64];
};