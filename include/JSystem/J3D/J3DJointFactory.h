#ifndef _JSYSTEM_J3D_J3DJOINTFACTORY_H
#define _JSYSTEM_J3D_J3DJOINTFACTORY_H

#include "JSystem/J3D/J3DFileBlock.h"
#include "JSystem/J3D/J3DJoint.h"
#include "JSystem/J3D/J3DTypes.h"
#include "JSystem/JGeometry.h"
#include "types.h"

struct J3DJointInitData {
	u16 mKind;                       // _00
	s8 mIgnoreParentScaling;         // _02
	J3DTransformInfo mTransformInfo; // _04
	f32 mRadius;                     // _24, bounding sphere radius
	Vec mMin;          				 // _28
	Vec mMax;          				 // _34
};

struct J3DJointFactory {
	J3DJointFactory(const J3DJointBlock& block);

	J3DJoint* create(int jointIndex);
	
	Vec& getMin(int no) const { return mInitData[mIndexMap[no]].mMin; }
    Vec& getMax(int no) const { return mInitData[mIndexMap[no]].mMax; }

	J3DJointInitData* mInitData; // _00
	u16* mIndexMap;              // _04, indices into mInitData, indexed by joint number.
};

#endif
