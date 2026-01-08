#include "JSystem/J3D/J3DMtxBuffer.h"
#include "JSystem/J3D/J3DMaterial.h"
#include "JSystem/J3D/J3DModel.h"
#include "JSystem/J3D/J3DModelLoader.h"
#include "JSystem/J3D/J3DSys.h"
#include "JSystem/J3D/J3DTexGenBlock.h"
#include "JSystem/J3D/J3DTransform.h"
#include "JSystem/JKernel/JKRHeap.h"
#include "RevoSDK/mtx.h"
#include "types.h"

Mtx J3DMtxBuffer::sNoUseDrawMtx;
Mtx33 J3DMtxBuffer::sNoUseNrmMtx;

Mtx* J3DMtxBuffer::sNoUseDrawMtxPtr  = &J3DMtxBuffer::sNoUseDrawMtx;
Mtx33* J3DMtxBuffer::sNoUseNrmMtxPtr = &J3DMtxBuffer::sNoUseNrmMtx;

static f32 J3DUnit01[] = { 0.0f, 1.0f };

/**
 * @note Address: 0x800888D8
 * @note Size: 0x40
 */
void J3DMtxBuffer::initialize()
{
	mJointTree              = nullptr;
	mScaleFlags             = nullptr;
	mEnvelopeScaleFlags     = 0;
	mWorldMatrices          = nullptr;
	mWeightEnvelopeMatrices = nullptr;
	mDrawMatrices[0]        = nullptr;
	mDrawMatrices[1]        = nullptr;
	mNormMatrices[0]        = nullptr;
	mNormMatrices[1]        = nullptr;
	mBumpMatrices[0]        = 0;
	mBumpMatrices[1]        = nullptr;
	mViewCount              = 1;
	mCurrentViewNumber      = 0;
	mUserAnmMtx      		= nullptr;
}

/**
 * @note Address: 0x80088918
 * @note Size: 0x11C
 */
int J3DMtxBuffer::create(J3DModelData* data, u32 viewNum)
{
	mViewCount = viewNum;
	mJointTree = &data->getJointTree();
	int result = createAnmMtx(data);
	if (result != JET_Success) {
		return result;
	}
	result = createWeightEnvelopeMtx(data);
	if (result != JET_Success) {
		return result;
	}
	if (data->checkFlag(J3DMLF_NoMatrixTransform)) {
		setNoUseDrawMtx();

	} else {
		switch (getMdlDataFlag_MtxLoadType(data->getFlag())) {
		case 0x10:
			result = setNoUseDrawMtx();
			break;
		case 0x0:
		default:
			result = createDoubleDrawMtx(data, viewNum);
			break;
		}
	}
	if (result != JET_Success) {
		return result;
	}
	if (data->getFlag() & J3DMLF_UseImmediateMtx) {
		data->setBumpFlag(0);
	} else {
		result = createBumpMtxArray(data, viewNum);
		if (result != JET_Success) {
			return result;
		}
	}
	return result;
}

/**
 * @note Address: 0x80088A34
 * @note Size: 0x5C
 */
int J3DMtxBuffer::createAnmMtx(J3DModelData* data)
{
	if (data->mJointTree.getJointNum()) {
		mScaleFlags    = new u8[data->mJointTree.getJointNum()];
		mWorldMatrices = new Mtx[data->mJointTree.getJointNum()];
		mUserAnmMtx    = mWorldMatrices;
	}
	return JET_Success;
}

/**
 * @note Address: 0x80088A90
 * @note Size: 0x5C
 */
int J3DMtxBuffer::createWeightEnvelopeMtx(J3DModelData* data)
{
	if (data->mJointTree.getWEvlpMtxNum()) {
		mEnvelopeScaleFlags     = new u8[data->mJointTree.getWEvlpMtxNum()];
		mWeightEnvelopeMatrices = new Mtx[data->mJointTree.getWEvlpMtxNum()];
	}
	return JET_Success;
}

/**
 * @note Address: 0x80088AEC
 * @note Size: 0x2C
 */
int J3DMtxBuffer::setNoUseDrawMtx()
{
	mDrawMatrices[1] = &sNoUseDrawMtxPtr;
	mDrawMatrices[0] = &sNoUseDrawMtxPtr;
	mNormMatrices[1] = &sNoUseNrmMtxPtr;
	mNormMatrices[0] = &sNoUseNrmMtxPtr;
	mBumpMatrices[1] = nullptr;
	mBumpMatrices[0] = nullptr;
	return JET_Success;
}

/**
 * @note Address: 0x80088B18
 * @note Size: 0xE0
 */
int J3DMtxBuffer::createDoubleDrawMtx(J3DModelData* data, u32 num)
{
	if (num != 0) {
		for (int i = 0; i < 2; i++) {
			mDrawMatrices[i] = new Mtx*[num];
			mNormMatrices[i] = new Mtx33*[num];
			mBumpMatrices[i] = nullptr;
		}
	}

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < num; j++) {
			if (data->getDrawMtxNum()) {
				mDrawMatrices[i][j] = new (0x20) Mtx[data->getDrawMtxNum()];
				mNormMatrices[i][j] = new (0x20) Mtx33[data->getDrawMtxNum()];
			}
		}
	}
	return JET_Success;
}

/**
 * @note Address: 0x80088BF8
 * @note Size: 0x214
 */
int J3DMtxBuffer::createBumpMtxArray(J3DModelData* data, u32 viewNum)
{
	if (data->getModelDataType() == J3DMLF_None) {
		u32 bumpMtxCount  = 0;
		u16 materialCount = data->getMaterialNum();
		u16 v1            = 0;
		for (u16 i = 0; i < materialCount; i++) {
			J3DMaterial* material = data->getMaterialNodePointer(i);
			if (material->getNBTScale()->mHasScale == 1) {
				bumpMtxCount += material->getShape()->countBumpMtxNum();
				v1++;
			}
		}
		if ((u16)bumpMtxCount != 0 && viewNum != 0) {
			for (int i = 0; i < 2; i++) {
				mBumpMatrices[i] = new Mtx33**[v1];
			}
		}
		for (int i = 0; i < 2; i++) {
			u16 materialCount = data->getMaterialNum();
			int shapeCount    = 0;
			for (u16 matIndex = 0; matIndex < materialCount; matIndex++) {
				J3DMaterial* material = data->getMaterialNodePointer(matIndex);
				if (material->getNBTScale()->mHasScale == 1) {
					mBumpMatrices[i][shapeCount] = new Mtx33*[viewNum];
					material->getShape()->setBumpMtxOffset(shapeCount);
					shapeCount += 1;
				}
			}
		}
		for (int i = 0; i < 2; i++) {
			u16 materialCount = data->getMaterialNum();
			int j             = 0;
			for (u16 matIndex = 0; matIndex < materialCount; matIndex++) {
				J3DMaterial* material = data->getMaterialNodePointer(matIndex);
				if (material->getNBTScale()->mHasScale == 1) {
					for (int k = 0; k < viewNum; k++) {
						mBumpMatrices[i][j][k] = new (0x20) Mtx33[data->getDrawMtxNum()];
					}
					j++;
				}
			}
		}
		if (v1) {
			data->setBumpFlag(1);
		}
	}
	return JET_Success;
}

/**
 * @note Address: 0x80088E0C
 * @note Size: 0x220
 */
void J3DMtxBuffer::calcWeightEnvelopeMtx()
{
	register MtxP weightAnmMtx;
    register Mtx* worldMtx;
    register Mtx* invMtx;
    register f32 weight;
    int idx;
    int j;
    int mixNum;
    int i;
    int max;
    u16* indices;
    f32* weights;
    u8* scaleFlags;

#if !__MWERKS__
    register Mtx mtx;
#else
    register f32 var_f1;
    register f32 var_f2;
    register f32 var_f3;
    register f32 var_f4;
    register f32 var_f5;
    register f32 var_f6;
    register f32 var_f7;
    register f32 var_f8;
    register f32 var_f9;
    register f32 var_f10;
    register f32 var_f11;
    register f32 var_f12;
    register f32 var_f13;
    register f32 var_f31;
    register f32 var_f30;
    register f32 var_f29;
    register f32 var_f28;
    register f32 var_f27;
    register f32* var_r7 = J3DUnit01;
#endif

    i = -1;
    max = mJointTree->getWEvlpMtxNum();
    indices = mJointTree->getWEvlpMixIndex() - 1;
    weights = mJointTree->getWEvlpMixWeight() - 1;

    #if __MWERKS__
    asm {
        psq_l var_f27, 0x0(var_r7), 0, 0 /* qr0 */
        ps_merge00 var_f10, var_f27, var_f27
        ps_merge00 var_f12, var_f27, var_f27
        ps_merge00 var_f31, var_f27, var_f27
    }
    #endif

    while (++i < max) {
        scaleFlags = &mEnvelopeScaleFlags[i];
        *scaleFlags = 1;
        weightAnmMtx = mWeightEnvelopeMatrices[i];

        #if !__MWERKS__
        weightAnmMtx[0][0] = weightAnmMtx[0][1] = weightAnmMtx[0][2] = weightAnmMtx[0][3] = 
        weightAnmMtx[1][0] = weightAnmMtx[1][1] = weightAnmMtx[1][2] = weightAnmMtx[1][3] = 
        weightAnmMtx[2][0] = weightAnmMtx[2][1] = weightAnmMtx[2][2] = weightAnmMtx[2][3] = 0.0f;
        #else
        asm {
            ps_merge00 var_f9, var_f27, var_f27
            ps_merge00 var_f11, var_f27, var_f27
            ps_merge00 var_f13, var_f27, var_f27
        }
        #endif

        j = 0;
        mixNum = mJointTree->getWEvlpMixMtxNum(i);
        do {
            idx = *++indices;
            worldMtx = &mWorldMatrices[idx];
            invMtx = &mJointTree->getInvJointMtx((u16)idx);

            #if !__MWERKS__
            C_MTXConcat(*worldMtx, *invMtx, mtx);
            #else
            // Fakematch? Doesn't match if worldMtx and invMtx are used directly.
            register void* var_r5 = worldMtx;
            register void* var_r6 = invMtx;
            asm {
                psq_l var_f2, 0x0(var_r6), 0, 0 /* qr0 */
                psq_l var_f1, 0x0(var_r5), 0, 0 /* qr0 */
                psq_l var_f3, 0x10(var_r5), 0, 0 /* qr0 */
                psq_l var_f5, 0x20(var_r5), 0, 0 /* qr0 */
                ps_muls0 var_f8, var_f2, var_f1
                psq_l var_f6, 0x10(var_r6), 0, 0 /* qr0 */
                ps_muls0 var_f30, var_f2, var_f3
                ps_muls0 var_f29, var_f2, var_f5
                psq_l var_f7, 0x20(var_r6), 0, 0 /* qr0 */
                ps_madds1 var_f8, var_f6, var_f1, var_f8
                psq_l var_f2, 0x8(var_r5), 0, 0 /* qr0 */
                ps_madds1 var_f30, var_f6, var_f3, var_f30
                psq_l var_f4, 0x18(var_r5), 0, 0 /* qr0 */
                ps_madds1 var_f29, var_f6, var_f5, var_f29
                psq_l var_f6, 0x28(var_r5), 0, 0 /* qr0 */
                ps_madds0 var_f8, var_f7, var_f2, var_f8
            }
            #endif

            weight = *++weights;

            #if !__MWERKS__
            weightAnmMtx[0][0] += mtx[0][0] * weight;
            weightAnmMtx[0][1] += mtx[0][1] * weight;
            weightAnmMtx[0][2] += mtx[0][2] * weight;
            weightAnmMtx[0][3] += mtx[0][3] * weight;
            weightAnmMtx[1][0] += mtx[1][0] * weight;
            weightAnmMtx[1][1] += mtx[1][1] * weight;
            weightAnmMtx[1][2] += mtx[1][2] * weight;
            weightAnmMtx[1][3] += mtx[1][3] * weight;
            weightAnmMtx[2][0] += mtx[2][0] * weight;
            weightAnmMtx[2][1] += mtx[2][1] * weight;
            weightAnmMtx[2][2] += mtx[2][2] * weight;
            weightAnmMtx[2][3] += mtx[2][3] * weight;
            #else
            asm {
                ps_madds0 var_f30, var_f7, var_f4, var_f30
                ps_madds0 var_f29, var_f7, var_f6, var_f29
                psq_l var_f7, 0x8(var_r6), 0, 0 /* qr0 */
                ps_madds0 var_f9, var_f8, weight, var_f9
                ps_madds0 var_f11, var_f30, weight, var_f11
                ps_madds0 var_f13, var_f29, weight, var_f13
                psq_l var_f8, 0x18(var_r6), 0, 0 /* qr0 */
                ps_muls0 var_f30, var_f7, var_f1
                ps_muls0 var_f29, var_f7, var_f3
                ps_muls0 var_f28, var_f7, var_f5
                psq_l var_f7, 0x28(var_r6), 0, 0 /* qr0 */
                psq_st var_f9, 0x0(weightAnmMtx), 0, 0 /* qr0 */
                ps_madds1 var_f30, var_f8, var_f1, var_f30
                ps_madds1 var_f29, var_f8, var_f3, var_f29
                ps_madds1 var_f28, var_f8, var_f5, var_f28
                ps_madds0 var_f30, var_f7, var_f2, var_f30
                ps_madds0 var_f29, var_f7, var_f4, var_f29
                ps_madds0 var_f28, var_f7, var_f6, var_f28
                psq_st var_f11, 0x10(weightAnmMtx), 0, 0 /* qr0 */
                psq_st var_f13, 0x20(weightAnmMtx), 0, 0 /* qr0 */
                ps_madd var_f30, var_f27, var_f2, var_f30
                ps_madd var_f29, var_f27, var_f4, var_f29
                ps_madd var_f28, var_f27, var_f6, var_f28
                ps_madds0 var_f10, var_f30, weight, var_f10
                ps_madds0 var_f12, var_f29, weight, var_f12
                ps_madds0 var_f31, var_f28, weight, var_f31
            }
            #endif

            *scaleFlags &= mScaleFlags[idx];
        } while (++j < mixNum);

        #if __MWERKS__
        asm {
            psq_st var_f10, 0x8(weightAnmMtx), 0, 0 /* qr0 */
            ps_merge00 var_f10, var_f27, var_f27
            psq_st var_f12, 0x18(weightAnmMtx), 0, 0 /* qr0 */
            ps_merge00 var_f12, var_f27, var_f27
            psq_st var_f31, 0x28(weightAnmMtx), 0, 0 /* qr0 */
            ps_merge00 var_f31, var_f27, var_f27
        }
        #endif
	}

	/*	
	stwu     r1, -0xa0(r1)
	stfd     f31, 0x90(r1)
	psq_st   f31, 152(r1), 0, qr0
	stfd     f30, 0x80(r1)
	psq_st   f30, 136(r1), 0, qr0
	stfd     f29, 0x70(r1)
	psq_st   f29, 120(r1), 0, qr0
	stfd     f28, 0x60(r1)
	psq_st   f28, 104(r1), 0, qr0
	stfd     f27, 0x50(r1)
	psq_st   f27, 88(r1), 0, qr0
	stfd     f26, 0x40(r1)
	psq_st   f26, 72(r1), 0, qr0
	stfd     f25, 0x30(r1)
	psq_st   f25, 56(r1), 0, qr0
	stfd     f24, 0x20(r1)
	psq_st   f24, 40(r1), 0, qr0
	stw      r31, 0x1c(r1)
	stw      r30, 0x18(r1)
	stw      r29, 0x14(r1)
	addi     r4, r13, J3DUnit01@sda21
	lwz      r6, 0(r3)
	psq_l    f24, 0(r4), 0, qr0
	li       r31, -1
	lwz      r5, 0x24(r6)
	li       r8, -48
	lwz      r4, 0x28(r6)
	ps_merge00 f11, f24, f24
	ps_merge00 f13, f24, f24
	lhz      r7, 0x1e(r6)
	ps_merge00 f30, f24, f24
	addi     r30, r5, -2
	addi     r29, r4, -4
	b        lbl_80088FC8

lbl_80088E94:
	lwz      r6, 8(r3)
	li       r4, 1
	ps_merge00 f10, f24, f24
	clrlwi   r0, r31, 0x10
	stbx     r4, r6, r31
	ps_merge00 f12, f24, f24
	ps_merge00 f31, f24, f24
	li       r11, 0
	lwz      r4, 0(r3)
	lwz      r5, 0x10(r3)
	lwz      r4, 0x20(r4)
	add      r9, r5, r8
	lbzx     r12, r4, r0

lbl_80088EC8:
	lhzu     r10, 2(r30)
	addi     r11, r11, 1
	lwz      r5, 0(r3)
	cmpw     r11, r12
	mulli    r4, r10, 0x30
	lwz      r0, 0xc(r3)
	lwz      r5, 0x2c(r5)
	lfsu     f0, 4(r29)
	add      r5, r5, r4
	add      r4, r0, r4
	psq_l    f2, 0(r5), 0, qr0
	psq_l    f1, 0(r4), 0, qr0
	psq_l    f3, 16(r4), 0, qr0
	ps_muls0 f29, f2, f1
	psq_l    f8, 16(r5), 0, qr0
	ps_muls0 f27, f2, f3
	psq_l    f5, 32(r4), 0, qr0
	psq_l    f7, 8(r5), 0, qr0
	ps_muls0 f25, f2, f5
	ps_madds1 f29, f8, f1, f29
	psq_l    f9, 32(r5), 0, qr0
	psq_l    f2, 8(r4), 0, qr0
	ps_madds1 f27, f8, f3, f27
	psq_l    f4, 24(r4), 0, qr0
	ps_madds0 f29, f9, f2, f29
	ps_madds1 f25, f8, f5, f25
	psq_l    f6, 40(r4), 0, qr0
	ps_madds0 f27, f9, f4, f27
	psq_l    f8, 24(r5), 0, qr0
	ps_muls0 f28, f7, f1
	ps_muls0 f26, f7, f3
	ps_madds0 f25, f9, f6, f25
	psq_l    f9, 40(r5), 0, qr0
	ps_madds0 f10, f29, f0, f10
	ps_muls0 f7, f7, f5
	ps_madds1 f28, f8, f1, f28
	ps_madds1 f26, f8, f3, f26
	psq_st   f10, 0(r9), 0, qr0
	ps_madds0 f12, f27, f0, f12
	ps_madds1 f7, f8, f5, f7
	ps_madds0 f28, f9, f2, f28
	ps_madds0 f26, f9, f4, f26
	psq_st   f12, 16(r9), 0, qr0
	ps_madds0 f31, f25, f0, f31
	ps_madds0 f7, f9, f6, f7
	ps_madd  f28, f24, f2, f28
	psq_st   f31, 32(r9), 0, qr0
	ps_madd  f26, f24, f4, f26
	ps_madd  f7, f24, f6, f7
	lwz      r4, 4(r3)
	ps_madds0 f11, f28, f0, f11
	lbzx     r5, r6, r31
	ps_madds0 f13, f26, f0, f13
	lbzx     r0, r4, r10
	ps_madds0 f30, f7, f0, f30
	and      r0, r5, r0
	stbx     r0, r6, r31
	blt      lbl_80088EC8
	psq_st   f11, 8(r9), 0, qr0
	ps_merge00 f11, f24, f24
	psq_st   f13, 24(r9), 0, qr0
	ps_merge00 f13, f24, f24
	psq_st   f30, 40(r9), 0, qr0
	ps_merge00 f30, f24, f24

lbl_80088FC8:
	addi     r31, r31, 1
	addi     r8, r8, 0x30
	cmpw     r31, r7
	blt      lbl_80088E94
	psq_l    f31, 152(r1), 0, qr0
	lfd      f31, 0x90(r1)
	psq_l    f30, 136(r1), 0, qr0
	lfd      f30, 0x80(r1)
	psq_l    f29, 120(r1), 0, qr0
	lfd      f29, 0x70(r1)
	psq_l    f28, 104(r1), 0, qr0
	lfd      f28, 0x60(r1)
	psq_l    f27, 88(r1), 0, qr0
	lfd      f27, 0x50(r1)
	psq_l    f26, 72(r1), 0, qr0
	lfd      f26, 0x40(r1)
	psq_l    f25, 56(r1), 0, qr0
	lfd      f25, 0x30(r1)
	psq_l    f24, 40(r1), 0, qr0
	lfd      f24, 0x20(r1)
	lwz      r31, 0x1c(r1)
	lwz      r30, 0x18(r1)
	lwz      r29, 0x14(r1)
	addi     r1, r1, 0xa0
	blr
	*/
}

/**
 * @note Address: 0x8008902C
 * @note Size: 0x304
 */
void J3DMtxBuffer::calcDrawMtx(u32 p1, const Vec& vec, const Mtx& mtx)
{
	switch (p1) {
	case 0: {
		Mtx* viewMtx = j3dSys.getViewMtx();
		u32 mtxNum   = mJointTree->getDrawFullWgtMtxNum();
		for (u16 i = 0; i < mtxNum; i++) {
			Mtx& drawMtx = *getDrawMtx(i);
			PSMTXConcat(*viewMtx, *(Mtx*)getAnmMtx(i), drawMtx);
		}

		if (mJointTree->getDrawMtxNum() > mtxNum) {
			J3DPSMtxArrayConcat(*viewMtx, mWeightEnvelopeMatrices[0], *getDrawMtx(mtxNum), mJointTree->getWEvlpMtxNum());
		}
	} break;

	case 1: {
		u32 mtxNum = mJointTree->getDrawFullWgtMtxNum();
		for (u16 i = 0; i < mtxNum; i++) {
			PSMTXCopy(*(Mtx*)getWeightAnmMtx(i), *getDrawMtx(i));
		}

		mtxNum = mJointTree->getDrawFullWgtMtxNum();
		for (u16 i = 0; i < mtxNum; i++) {
			PSMTXCopy(*(Mtx*)getAnmMtx(mJointTree->getDrawMtxIndex(i)), *getDrawMtx(i));
		}
	} break;

	case 2: {
		Mtx baseMtx;
		J3DCalcViewBaseMtx(*j3dSys.getViewMtx(), vec, mtx, baseMtx);
		u32 mtxNum = mJointTree->getDrawFullWgtMtxNum();
		for (u16 i = 0; i < mtxNum; i++) {
			PSMTXConcat(baseMtx, *(Mtx*)getAnmMtx(mJointTree->getDrawMtxIndex(i)), *getDrawMtx(i));
		}

		mtxNum = mJointTree->getDrawFullWgtMtxNum();
		if (mJointTree->getDrawMtxNum() > mtxNum) {
			J3DPSMtxArrayConcat(baseMtx, mWeightEnvelopeMatrices[0], *getDrawMtx(mtxNum), mJointTree->getWEvlpMtxNum());
		}
	} break;
	}
	/*
	stwu     r1, -0x80(r1)
	mflr     r0
	cmpwi    r4, 1
	stw      r0, 0x84(r1)
	stw      r31, 0x7c(r1)
	stw      r30, 0x78(r1)
	stw      r29, 0x74(r1)
	stw      r28, 0x70(r1)
	mr       r28, r3
	beq      lbl_80089118
	bge      lbl_80089064
	cmpwi    r4, 0
	bge      lbl_80089070
	b        lbl_80089310

lbl_80089064:
	cmpwi    r4, 3
	bge      lbl_80089310
	b        lbl_800891D0

lbl_80089070:
	lwz      r3, 0(r28)
	lis      r4, j3dSys@ha
	addi     r0, r4, j3dSys@l
	li       r29, 0
	lhz      r31, 0x36(r3)
	mr       r30, r0
	b        lbl_800890D0

lbl_8008908C:
	lwz      r4, 0(r28)
	clrlwi   r0, r29, 0x10
	lwz      r3, 0x30(r28)
	rlwinm   r6, r29, 1, 0xf, 0x1e
	lwz      r7, 0x3c(r4)
	mulli    r0, r0, 0x30
	lwz      r5, 0x18(r28)
	slwi     r4, r3, 2
	lhzx     r6, r7, r6
	mr       r3, r30
	lwzx     r4, r5, r4
	mulli    r6, r6, 0x30
	lwz      r7, 0xc(r28)
	add      r5, r4, r0
	add      r4, r7, r6
	bl       PSMTXConcat
	addi     r29, r29, 1

lbl_800890D0:
	clrlwi   r0, r29, 0x10
	cmplw    r0, r31
	blt      lbl_8008908C
	lwz      r7, 0(r28)
	lhz      r0, 0x34(r7)
	cmplw    r0, r31
	ble      lbl_80089310
	lwz      r4, 0x30(r28)
	mulli    r0, r31, 0x30
	lwz      r6, 0x18(r28)
	mr       r3, r30
	slwi     r5, r4, 2
	lwz      r4, 0x10(r28)
	lwzx     r5, r6, r5
	lhz      r6, 0x1e(r7)
	add      r5, r5, r0
	bl       J3DPSMtxArrayConcat__FPA4_fPA4_fPA4_fUl
	b        lbl_80089310

lbl_80089118:
	lwz      r3, 0(r28)
	li       r30, 0
	lhz      r29, 0x36(r3)
	b        lbl_80089168

lbl_80089128:
	lwz      r4, 0(r28)
	clrlwi   r0, r30, 0x10
	lwz      r3, 0x30(r28)
	rlwinm   r5, r30, 1, 0xf, 0x1e
	lwz      r6, 0x3c(r4)
	mulli    r0, r0, 0x30
	lwz      r4, 0x18(r28)
	slwi     r3, r3, 2
	lhzx     r5, r6, r5
	lwzx     r3, r4, r3
	mulli    r5, r5, 0x30
	lwz      r6, 0xc(r28)
	add      r4, r3, r0
	add      r3, r6, r5
	bl       PSMTXCopy
	addi     r30, r30, 1

lbl_80089168:
	clrlwi   r0, r30, 0x10
	cmplw    r0, r29
	blt      lbl_80089128
	lwz      r3, 0(r28)
	li       r30, 0
	lhz      r29, 0x36(r3)
	b        lbl_800891C0

lbl_80089184:
	lwz      r3, 0(r28)
	clrlwi   r6, r30, 0x10
	lwz      r5, 0x30(r28)
	mulli    r0, r6, 0x30
	lhz      r4, 0x36(r3)
	lwz      r3, 0x10(r28)
	slwi     r5, r5, 2
	add      r4, r6, r4
	lwz      r6, 0x18(r28)
	mulli    r4, r4, 0x30
	add      r3, r3, r0
	lwzx     r0, r6, r5
	add      r4, r0, r4
	bl       PSMTXCopy
	addi     r30, r30, 1

lbl_800891C0:
	clrlwi   r0, r30, 0x10
	cmplw    r0, r29
	blt      lbl_80089184
	b        lbl_80089310

lbl_800891D0:
	lfs      f2, 0(r5)
	lis      r3, j3dSys@ha
	lfs      f1, 0x10(r6)
	addi     r3, r3, j3dSys@l
	lfs      f3, 0(r6)
	addi     r4, r1, 8
	fmuls    f7, f1, f2
	lfs      f0, 0x20(r6)
	fmuls    f13, f3, f2
	lfs      f1, 4(r6)
	fmuls    f3, f0, f2
	lfs      f12, 4(r5)
	fmuls    f11, f1, f12
	lfs      f10, 8(r5)
	lfs      f2, 8(r6)
	addi     r5, r1, 0x38
	lfs      f1, 0x14(r6)
	fmuls    f9, f2, f10
	fmuls    f6, f1, f12
	lfs      f5, 0x18(r6)
	lfs      f2, 0x24(r6)
	lfs      f1, 0x28(r6)
	fmuls    f5, f5, f10
	fmuls    f2, f2, f12
	fmuls    f1, f1, f10
	lfs      f8, 0xc(r6)
	lfs      f4, 0x1c(r6)
	lfs      f0, 0x2c(r6)
	stfs     f13, 8(r1)
	stfs     f11, 0xc(r1)
	stfs     f9, 0x10(r1)
	stfs     f8, 0x14(r1)
	stfs     f7, 0x18(r1)
	stfs     f6, 0x1c(r1)
	stfs     f5, 0x20(r1)
	stfs     f4, 0x24(r1)
	stfs     f3, 0x28(r1)
	stfs     f2, 0x2c(r1)
	stfs     f1, 0x30(r1)
	stfs     f0, 0x34(r1)
	bl       PSMTXConcat
	lwz      r3, 0(r28)
	li       r30, 0
	lhz      r29, 0x36(r3)
	b        lbl_800892C8

lbl_80089284:
	lwz      r4, 0(r28)
	clrlwi   r0, r30, 0x10
	lwz      r3, 0x30(r28)
	rlwinm   r6, r30, 1, 0xf, 0x1e
	lwz      r7, 0x3c(r4)
	mulli    r0, r0, 0x30
	lwz      r5, 0x18(r28)
	slwi     r4, r3, 2
	lhzx     r6, r7, r6
	addi     r3, r1, 0x38
	lwzx     r4, r5, r4
	mulli    r6, r6, 0x30
	lwz      r7, 0xc(r28)
	add      r5, r4, r0
	add      r4, r7, r6
	bl       PSMTXConcat
	addi     r30, r30, 1

lbl_800892C8:
	clrlwi   r0, r30, 0x10
	cmplw    r0, r29
	blt      lbl_80089284
	lwz      r7, 0(r28)
	lhz      r3, 0x36(r7)
	lhz      r0, 0x34(r7)
	cmplw    r0, r3
	ble      lbl_80089310
	lwz      r4, 0x30(r28)
	mulli    r0, r3, 0x30
	lwz      r6, 0x18(r28)
	addi     r3, r1, 0x38
	slwi     r5, r4, 2
	lwz      r4, 0x10(r28)
	lwzx     r5, r6, r5
	lhz      r6, 0x1e(r7)
	add      r5, r5, r0
	bl       J3DPSMtxArrayConcat__FPA4_fPA4_fPA4_fUl

lbl_80089310:
	lwz      r0, 0x84(r1)
	lwz      r31, 0x7c(r1)
	lwz      r30, 0x78(r1)
	lwz      r29, 0x74(r1)
	lwz      r28, 0x70(r1)
	mtlr     r0
	addi     r1, r1, 0x80
	blr
	*/
}

/**
 * @note Address: 0x80089330
 * @note Size: 0x1BC
 */
void J3DMtxBuffer::calcNrmMtx()
{
	u16 mtxNum = mJointTree->mMtxData.mCount;

	for (u16 i = 0; i < mtxNum; i++) {
		if (mJointTree->getDrawMtxFlag(i) == 0) {
			if (getScaleFlag(mJointTree->getDrawMtxIndex(i)) == 1) {
				setNrmMtx(i, *getDrawMtx(i));
			} else {
				J3DPSCalcInverseTranspose(*getDrawMtx(i), *getNrmMtx(i));
			}
			continue;
		}

		if (getEnvScaleFlag(mJointTree->getDrawMtxIndex(i)) == 1) {
			setNrmMtx(i, *getDrawMtx(i));
			continue;
		}

		J3DPSCalcInverseTranspose(*getDrawMtx(i), *getNrmMtx(i));
	}
}

/**
 * @note Address: 0x800894EC
 * @note Size: 0x178
 */
void J3DMtxBuffer::calcBBoardMtx()
{
	u32 mtxNum = mJointTree->getDrawMtxNum();
	for (u16 i = 0; i < mtxNum; i++) {
		if (mJointTree->getDrawMtxFlag(i)) {
			continue;
		}

		if (mJointTree->getJointNodePointer((u32)(mJointTree->getDrawMtxIndex(i)))->getMtxType() == 1) {
			Mtx& drawMtx = *getDrawMtx(i);
			J3DCalcBBoardMtx(drawMtx);
			Mtx33& nrmMtx = *getNrmMtx(i);
			nrmMtx[0][0]  = (drawMtx[0][0] != 0.0f) ? 1.0f / drawMtx[0][0] : 0.0f;
			nrmMtx[0][1]  = 0.0f;
			nrmMtx[0][2]  = 0.0f;

			nrmMtx[1][0] = 0.0f;
			nrmMtx[1][1] = (drawMtx[1][1] != 0.0f) ? 1.0f / drawMtx[1][1] : 0.0f;
			nrmMtx[1][2] = 0.0f;

			nrmMtx[2][0] = 0.0f;
			nrmMtx[2][1] = 0.0f;
			nrmMtx[2][2] = (drawMtx[2][2] != 0.0f) ? 1.0f / drawMtx[2][2] : 0.0f;
			continue;
		}

		if (mJointTree->getJointNodePointer((int)mJointTree->getDrawMtxIndex(i))->getMtxType() == 2) {
			Mtx& drawMtx = *getDrawMtx(i);
			J3DCalcYBBoardMtx(drawMtx);
			J3DPSCalcInverseTranspose(drawMtx, *getNrmMtx(i));
		}
	}
}

void J3DCalcViewBaseMtx(Mtx view, Vec const& scale, const Mtx& base, Mtx dst) {
    Mtx m;

	m[0][0] = base[0][0] * scale.x;
	m[0][1] = base[0][1] * scale.y;
	m[0][2] = base[0][2] * scale.z;
	m[0][3] = base[0][3];

	m[1][0] = base[1][0] * scale.x;
	m[1][1] = base[1][1] * scale.y;
	m[1][2] = base[1][2] * scale.z;
	m[1][3] = base[1][3];

	m[2][0] = base[2][0] * scale.x;
	m[2][1] = base[2][1] * scale.y;
	m[2][2] = base[2][2] * scale.z;
	m[2][3] = base[2][3];

	PSMTXConcat(view, m, dst);
}
