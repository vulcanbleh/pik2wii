#include "JSystem/J3D/J3DAnmLoader.h"
#include "JSystem/JKernel/JKRDvdRipper.h"
#include "P2Macros.h"
#include "SysShape/AnimInfo.h"
#include "SysShape/AnimMgr.h"
#include "SysShape/Animator.h"
#include "SysShape/Joint.h"
#include "SysShape/KeyEvent.h"
#include "SysShape/Model.h"
#include "SysShape/MotionListener.h"
#include "SysShape/MtxObject.h"
#include "nans.h"

// TODO: fix this up
static void __Print(const char** fmt, ...)
{
	*fmt = "sysShape";
}

namespace SysShape {

bool Animator::verbose;

void Model::drawMesh()
{
	J3DShape::resetVcdVatCache();

	mJ3dModel->getModelData()->getShapeNodePointer(0)->loadPreDrawSetting();
	u16 shapeNum = mJ3dModel->getModelData()->getShapeNum();
	for (u16 i = 0; i < shapeNum; i++) {
		mJ3dModel->getShapePacket(i)->drawFast();
	}
}

/**
 * @todo: Documentation
 */
void Model::clearAnimatorAll()
{
	for (int i = 0; i < mJointCount; i++) {
		mJ3dModel->getModelData()->getJointTree().getJointNodePointer((u16)i)->mMtxCalc = nullptr;
	}
}

/**
 * @note Address: N/A
 * @note Size: 0x80
 */
void Model::setAnimatorAll(BaseAnimator&)
{
	// UNUSED FUNCTION
}

/**
 * @todo: Documentation
 */
void Animator::startAnim(int animID, MotionListener* listener)
{
	mAnimInfo = mAnimMgr->getAnimByID(animID);
	if (mAnimInfo == nullptr) {
		mAnimMgr->dump();
		JUT_PANICLINE(237, "go to hell !\n");
	}

	mTimer      = 0.0f;
	mCurAnimKey = mAnimInfo->getLowestAnimKey(0.0f);
	if (listener != nullptr) {
		mListener = listener;
	} else {
		mListener = nullptr;
	}

	mFlags.clear();
}

/**
 * @todo: Documentation
 */
void Animator::startExAnim(AnimInfo* info)
{
	mAnimInfo = info;
	mTimer    = 0.0f;
	mListener = nullptr;
	mFlags.clear();
	mCurAnimKey = nullptr;
	setFlag(Unk80);
	JUT_ASSERTLINE(269, verbose == 0, "OKOK\n");
}

/**
 * @todo: Documentation
 */
bool Animator::assertValid(Model* model)
{
	return true;
}

/**
 * @todo: Documentation
 */
void Animator::setCurrFrame(f32 timer)
{
	mTimer      = timer;
	mCurAnimKey = mAnimInfo->getLowestAnimKey(timer);
	mFlags.clear();
}

/**
 * @todo: Documentation
 */
void Animator::setFrameByKeyType(u32 id)
{
	JUT_ASSERTLINE(317, !isFlag(Unk80), "ExMotionErr::setFrameByKeyType(%d)\n", id);

	if (id == KEYEVENT_END) {
		setCurrFrame(mAnimInfo->mAnm->getTotalFrameCount() - 1.0f);
		return;
	}

	KeyEvent* evt = mAnimInfo->getAnimKeyByType(id);
	if (evt) {
		setCurrFrame(evt->mFrame);
	}
}

/**
 * @todo: Documentation
 */
void Animator::setLastFrame()
{
	if (mAnimInfo) {
		setCurrFrame(mAnimInfo->mAnm->getTotalFrameCount() - 1.0f);
	}
}

/**
 * @todo: Documentation
 */
void Animator::animate(f32 speed)
{
	if (!mAnimInfo) {
		return;
	}

	mTimer += speed;

	bool loopEndFound = false;
	while (!loopEndFound && mCurAnimKey && mCurAnimKey->getFrame() < (int)mTimer) {
		onKeyEventTrigger(mCurAnimKey);

		SysShape::KeyEvent* currentEv = mCurAnimKey;
		if (!currentEv) {
			break;
		}

		switch (currentEv->mType) {
		case KEYEVENT_LOOP_END:
			if (!isFlag(AnimFinishMotion)) {
				KeyEvent* start = mAnimInfo->getLastLoopStart(currentEv);
				if (start) {
					mTimer = start->getFrame();
				} else {
					mTimer = 0.0f;
					JUT_PANICLINE(386, "mismatch LOOP_START - LOOP_END\n");
				}

				loopEndFound = true;
				break;
			}
		}

		mCurAnimKey = (KeyEvent*)mCurAnimKey->mNext;
	}

	if (loopEndFound) {
		mCurAnimKey = mAnimInfo->getLowestAnimKey(mTimer);
	}

	s32 time = mAnimInfo->mAnm->getTotalFrameCount();
	if (mTimer >= time) {
		mTimer = time - 1.0f;

		if (mListener && !isFlag(AnimCompleted)) {
			KeyEvent event;
			event.mFrame   = (f32)mAnimInfo->mAnm->getTotalFrameCount();
			event.mType    = KEYEVENT_END;
			event.mAnimIdx = mAnimInfo->mId;
			setFlag(AnimCompleted);
			mListener->onKeyEvent(event);
		}
	}

	mAnimInfo->mAnm->setFrame((int)mTimer);
}

/**
 * @todo: Documentation
 */
BlendAnimator::BlendAnimator()
{
	mMtxCalc         = nullptr;
	mTimer           = 0.0f;
	mTimeMax         = 0.0f;
	mMotionListener  = nullptr;
	mIsBlendFinished = 0;
	mIsBlendEnabled  = false;
}

/**
 * @todo: Documentation
 */
void BlendAnimator::setAnimMgr(AnimMgr* mgr)
{
	mIsBlendEnabled = false;

	for (int i = 0; i < 2; i++) {
		mAnimators[i].mAnimMgr = mgr;
		mAnimators[i].startAnim(0, nullptr);
	}

	AnimInfo* info = mgr->getAnimByID(0);
	JUT_ASSERTLINE(459, info, "BlendAnimator : at least 1 motion is required!\n");

	mMtxCalc = J3DUNewMtxCalcAnm(mgr->mModel->mJ3dModel->mModelData->mJointTree.mFlags & J3DMLF_MtxTypeMask, info->mAnm, info->mAnm,
	                             nullptr, nullptr, MTXCalc_Blend);
}

/**
 * @note Address: N/A
 * @note Size: 0x124
 */
void BlendAnimator::setWeight(f32 weight)
{
	P2ASSERTBOOLLINE(471, 0.0f <= weight && weight <= 1.0f);

	// set animation A to the inverse of the weight
	mMtxCalc->setAnmTransform(0, mAnimators[0].getAnimation());
	mMtxCalc->setWeight(0, 1.0f - weight);

	// set animation B to the weight directly
	mMtxCalc->setAnmTransform(1, mAnimators[1].getAnimation());
	mMtxCalc->setWeight(1, weight);
}

/**
 * @todo: Documentation
 */
void BlendAnimator::startBlend(BlendFunction* func, f32 time, MotionListener* mlisten)
{
	mMotionListener = mlisten;
	mTimer          = 0.0f;
	mTimeMax        = time;

	setWeight(func->getValue(0.0f));

	mIsBlendFinished = false;
	mIsBlendEnabled  = true;
}

/**
 * @todo: Documentation
 */
void BlendAnimator::endBlend()
{
	mIsBlendEnabled  = false;
	mIsBlendFinished = false;
	mTimer           = 0.0f;
}

/**
 * @todo: Documentation
 */
void BlendAnimator::animate(BlendFunction* func, f32 dt, f32 anim0Dt, f32 anim1Dt)
{
	if (mIsBlendEnabled) {
		mAnimators[0].animate(anim0Dt);
		mAnimators[1].animate(anim1Dt);
		mTimer += dt;
		if (mTimer >= mTimeMax) {
			mTimer = mTimeMax;
			if (!mIsBlendFinished) {
				mIsBlendFinished = true;
				if (mMotionListener) {
					KeyEvent event;
					event.mFrame = mTimeMax;
					event.mType  = KEYEVENT_END_BLEND;
					mMotionListener->onKeyEvent(event);
				}
			}
		}
		setWeight(func->getValue(mTimer / mTimeMax));
	} else {
		mAnimators[0].animate(anim0Dt);
	}
}

/**
 * @todo: Documentation
 */
J3DMtxCalc* BlendAnimator::getCalc()
{
	if (mIsBlendEnabled) {
		return mMtxCalc;
	}
	return mAnimators[0].getCalc();
}

/**
 * @todo: Documentation
 */
void Joint::init(u16 index, Model* model, J3DJoint* j3dJoint)
{
	mJointIndex = index;
	mModel      = model;
	mJ3d        = j3dJoint;
	mMin.x      = j3dJoint->mMin.x;
	mMin.y      = j3dJoint->mMin.y;
	mMin.z      = j3dJoint->mMin.z;
	mMax.x      = j3dJoint->mMax.x;
	mMax.y      = j3dJoint->mMax.y;
	mMax.z      = j3dJoint->mMax.z;
	mName       = model->mJ3dModel->mModelData->mJointTree.mNametab->getName(mJointIndex);
}

/**
 * @todo: Documentation
 */
Matrixf* Joint::getWorldMatrix()
{
	return mModel->getJ3DModel()->mMtxBuffer->getWorldMatrix(mJointIndex);
}

/**
 * @note Address: N/A
 * @note Size: 0xC
 */
void Joint::setCallback(JointCallback* cb)
{
	// UNUSED FUNCTION
}

/**
 * @note Address: N/A
 * @note Size: 0x78
 */
void JointCallback::calc()
{
	// UNUSED FUNCTION
}

/**
 * @note Address: N/A
 * @note Size: 0x5C
 */
void AnimInfo::attach(Model*, void*)
{
	// UNUSED FUNCTION
}

/**
 * @note Returns the lowest anim key after the given minimum frame.
 */
KeyEvent* AnimInfo::getLowestAnimKey(f32 minimumFrame)
{
	f32 lowestFrame     = FLOAT_DIST_MAX;
	KeyEvent* lowestKey = nullptr;
	FOREACH_NODE(KeyEvent, mKeyEvent.mChild, key)
	{
		int frame      = key->getFrame();
		f32 framefloat = frame;

		if (frame >= (int)minimumFrame && framefloat < lowestFrame) {
			lowestFrame = framefloat;
			lowestKey   = key;
		}
	}

	return lowestKey;
}

/**
 * @note Address: N/A
 * @note Size: 0x2C
 */
void AnimInfo::dump()
{
	// UNUSED FUNCTION
}

/**
 * @note Address: N/A
 * @note Size: 0x64
 */
void AnimInfo::getLastLoopStart(f32)
{
	// UNUSED FUNCTION
}

/**
 * @todo: Documentation
 */
KeyEvent* AnimInfo::getLastLoopStart(KeyEvent* key)
{
	FOREACH_NODE_REVERSE(KeyEvent, key->mPrev, prev)
	{
		if (prev->mType == KEYEVENT_LOOP_START) {
			return prev;
		}
	}
	return nullptr;
}

/**
 * @todo: Documentation
 */
KeyEvent* AnimInfo::getAnimKeyByType(u32 type)
{
	FOREACH_NODE(KeyEvent, mKeyEvent.mChild, next)
	{
		if (next->mType == type) {
			return next;
		}
	}
	return nullptr;
}

/**
 * @todo: Documentation
 */
void AnimInfo::read(Stream& input)
{
	readEditor(input);
}

/**
 * @todo: Documentation
 */
void AnimInfo::readEditor(Stream& input)
{
	mDevelopPath = input.readString(nullptr, 0);
	mName        = input.readString(nullptr, 0);
	while (input.eof() == 0) {
		int frame = input.readInt();
		if (frame == -1) {
			return;
		}
		KeyEvent* key = new KeyEvent;
		key->mFrame   = frame;
		key->mType    = input.readInt();
		mKeyEvent.add(key);
	}
	JUT_PANICLINE(858, "reached eof\n");
}

/**
 * @todo: Documentation
 */
AnimMgr* AnimMgr::load(char* path, J3DModelData* modelData, JKRFileLoader* fileLoader)
{
	void* data = JKRDvdRipper::loadToMainRAM(path, nullptr, Switch_0, 0, JKRHeap::sSystemHeap, JKRDvdRipper::ALLOC_DIR_BOTTOM, 0, nullptr,
	                                         nullptr);
	if (data == nullptr) {
		return nullptr;
	}
	RamStream input(data, -1);
	input.setMode(STREAM_MODE_TEXT, 1);
	AnimMgr* mgr = new AnimMgr;
	mgr->load(input, modelData, fileLoader, nullptr);
	delete[] data;
	return mgr;
}

/**
 * @todo: Documentation
 */
void AnimMgr::dump()
{
	for (int i = 0; i < mCount; i++) {
		getAnimByID(i);
	}
}

/**
 * @note Address: N/A
 * @note Size: 0xCC
 */
AnimMgr* AnimMgr::load(Stream&, Model*, JKRFileLoader*, char*)
{
	// UNUSED FUNCTION
}

/**
 * @note Address: N/A
 * @note Size: 0x5C
 */
JointCallback::~JointCallback()
{
	// UNUSED FUNCTION
}

/**
 * @note Address: N/A
 * @note Size: 0x4
 */
void JointCallback::init(Vec const&, const Mtx&)
{
	// UNUSED FUNCTION
}

} // namespace SysShape
