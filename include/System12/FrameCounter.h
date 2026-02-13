#ifndef _SYSTEM12_FRAME_COUNTER_H
#define _SYSTEM12_FRAME_COUNTER_H

#include <RevoSDK/wpad.h>
#include <egg/core/eggSingleton.h>
#include <egg/core/eggBitFlag.h>


namespace System12 {

class FrameCounter {
public:	
	enum eType {
		TYPE_Unk0 = 0,
		TYPE_Unk1 = 1,
	};
	
	
	FrameCounter();
	
	virtual void play(eType, f32);
	virtual void playFromCurrent(eType, f32);
	virtual void playFromCurrentByDiff(eType, f32, f32);
	virtual void playFromCurrentToTrg(eType, f32, f32);
	virtual void stop(f32);
	virtual void stopCurrent();
	virtual void stopAtEnd();
	virtual void calc();
	virtual void setCurrentFrame(f32);
	
	void resetUserFrameRange();
	u16 isEnd() const;
	
	//unused/inlined:
	
	void setUserFrameRange(f32, f32);
	void setupMaxFrames(f32);
	f32 getUserMaxFrame() const;
	f32 getSpeed() const;
	f32 getCurrentFrame() const;
	f32 getMaxFrames() const;
	eType getType() const;
	u16 isTurnedMax() const;
	u16 isTurnedMin() const;
	u16 isEndDiff() const;
	
	// _00     = VTBL
	f32 mCurrentFrame; 				// _04
	f32 _08; 						// _08
	f32 mDiff; 						// _0C
	f32 mMaxFrames; 				// _10
	f32 mUserFrameRangeStart; 		// _14
	f32 mUserFrameRangeEnd; 		// _18
	eType mType; 					// _1C
	u32 _20; 						// _20
	EGG::TBitFlag<u16> mFlags; 		// _24
};


} // namespace System12

#endif
