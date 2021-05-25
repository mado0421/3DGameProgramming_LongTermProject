#pragma once

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
			boneIdxStencil.push_back(pair<int, int>(2, stencil));	// Spine
			boneIdxStencil.push_back(pair<int, int>(3, stencil));	// Spine1
			boneIdxStencil.push_back(pair<int, int>(4, stencil));	// Spine2
			boneIdxStencil.push_back(pair<int, int>(5, stencil));	// Neck
			boneIdxStencil.push_back(pair<int, int>(6, stencil));	// L Clavicle
			boneIdxStencil.push_back(pair<int, int>(7, stencil));	// L UpperArm
			boneIdxStencil.push_back(pair<int, int>(8, stencil));	// L Forearm
			boneIdxStencil.push_back(pair<int, int>(9, stencil));	// L Hand
			boneIdxStencil.push_back(pair<int, int>(10, stencil));	// L Finger0
			boneIdxStencil.push_back(pair<int, int>(11, stencil));	// L Finger01
			boneIdxStencil.push_back(pair<int, int>(13, stencil));	// L Finger1
			boneIdxStencil.push_back(pair<int, int>(14, stencil));	// L Finger11
			boneIdxStencil.push_back(pair<int, int>(16, stencil));	// L Finger2
			boneIdxStencil.push_back(pair<int, int>(17, stencil));	// L Finger21
			boneIdxStencil.push_back(pair<int, int>(19, stencil));	// L Finger3
			boneIdxStencil.push_back(pair<int, int>(20, stencil));	// L Finger31
			boneIdxStencil.push_back(pair<int, int>(22, stencil));	// L Finger4
			boneIdxStencil.push_back(pair<int, int>(23, stencil));	// L Finger41
			boneIdxStencil.push_back(pair<int, int>(25, stencil));	// R Clavicle
			boneIdxStencil.push_back(pair<int, int>(26, stencil));	// R UpperArm
			boneIdxStencil.push_back(pair<int, int>(27, stencil));	// R Forearm
			boneIdxStencil.push_back(pair<int, int>(28, stencil));	// R Hand
			boneIdxStencil.push_back(pair<int, int>(29, stencil));	// R Finger0
			boneIdxStencil.push_back(pair<int, int>(30, stencil));	// R Finger01
			boneIdxStencil.push_back(pair<int, int>(32, stencil));	// R Finger1
			boneIdxStencil.push_back(pair<int, int>(33, stencil));	// R Finger11
			boneIdxStencil.push_back(pair<int, int>(35, stencil));	// R Finger2
			boneIdxStencil.push_back(pair<int, int>(36, stencil));	// R Finger21
			boneIdxStencil.push_back(pair<int, int>(38, stencil));	// R Finger3
			boneIdxStencil.push_back(pair<int, int>(39, stencil));	// R Finger31
			boneIdxStencil.push_back(pair<int, int>(41, stencil));	// R Finger4
			boneIdxStencil.push_back(pair<int, int>(42, stencil));	// R Finger41
			boneIdxStencil.push_back(pair<int, int>(44, stencil));	// Head
			break;
		case BoneMask::PreDefined::eLowerBody:
			boneIdxStencil.push_back(pair<int, int>(1, stencil));	// Pelvis
			boneIdxStencil.push_back(pair<int, int>(46, stencil));	// L Thigh
			boneIdxStencil.push_back(pair<int, int>(47, stencil));	// L Calf
			boneIdxStencil.push_back(pair<int, int>(48, stencil));	// L Foot
			boneIdxStencil.push_back(pair<int, int>(49, stencil));	// L Toe0
			boneIdxStencil.push_back(pair<int, int>(51, stencil));	// R Thigh
			boneIdxStencil.push_back(pair<int, int>(52, stencil));	// R Calf
			boneIdxStencil.push_back(pair<int, int>(53, stencil));	// R Foot
			boneIdxStencil.push_back(pair<int, int>(54, stencil));	// R Toe0
			break;
		default:
			break;
		}
	}

	vector<pair<int, int>> boneIdxStencil;
};