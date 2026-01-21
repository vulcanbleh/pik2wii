#ifndef _SYSTEM12_LAYOUT_H
#define _SYSTEM12_LAYOUT_H

#include <nw4r/ut.h>
#include <nw4r/lyt.h>
#include <egg/core/eggHeap.h>
#include <egg/math/eggMatrix.h>

#include <RevoSDK/gx.h>


class System12TextBox : public nw4r::lyt::TextBox {
public:

	System12TextBox(const nw4r::lyt::res::TextBox* pRes, const nw4r::lyt::ResBlockSet& rBlockSet)
		: nw4r::lyt::TextBox(pRes, rBlockSet)
		, _104(nullptr)
	{
		sysTextBoxInit();
	}
	
	
	virtual ~System12TextBox(); 									 // _08
    virtual u16 SetString(const wchar_t* pStr, u16 pos);        	 // _7C
    virtual u16 SetString(const wchar_t* pStr, u16 pos, u16 len);    // _80
	
	void sysTextBoxInit();
	void adjustImpl();
	
	int _104;        // _104
};	

namespace System12 {

class AnimTransformDisposer : public EGG::Disposer {
	AnimTransformDisposer() {}
	
	virtual ~AnimTransformDisposer(){}			// _08

};


class System12LayoutImpl : public nw4r::lyt::Layout {
public:
	System12LayoutImpl() {}
	
	virtual ~System12LayoutImpl();																// _08
	virtual bool Build(const void* pLytBinary, nw4r::lyt::ResourceAccessor* lytResBuf);			// _0C
	
	nw4r::lyt::Pane* buildPane(s32 kind, const void* pBinary,
                              const nw4r::lyt::ResBlockSet& rBlockSet);
};

class FontLessArcResourceAccessor : public nw4r::lyt::ArcResourceAccessor {
public:
	FontLessArcResourceAccessor() {}
	
	virtual ~FontLessArcResourceAccessor(){}								// _08
    virtual void* GetResource(u32 type, const char* pName, u32* pSize); 	// _0C
};	


class Layout {
public:
	Layout(EGG::Heap*);
	
	virtual void construct();		 	// _08	
	
	void createAnimTransform(const char*, bool);
	void createAnimator(nw4r::lyt::AnimTransform*);
	//void registerAnimator(Animator*);
	void clonePane(const char*, bool);
	void clonePaneRecursiveImpl(nw4r::lyt::Pane*, nw4r::lyt::Pane*);
	void clonePaneImpl(const char*);
	
	
	void bind(const char*);
	void bind(const char*, nw4r::lyt::Pane*, bool, bool);
	void calc();
	void calculateMtx();
	void calculateMtx(const EGG::Matrix34f&);
	void draw(_GXCullMode, bool);
	void draw(const EGG::Matrix34f&, _GXCullMode, bool);
	void allocatorAssert();
	

	int _00; 							// _00
	int _04;							// _04
	int _08; 							// _08
	int _0C; 							// _0C
	int _10; 							// _10
	

	//vtable is supposed to be at 0x14


	int _18; 							// _18
	System12LayoutImpl* _1C; 			// _1C
	FontLessArcResourceAccessor _20; 	// _20
	u8 _D0[0x10];
	int* _E0; 							// _E0
	EGG::Heap* mHeap;                   // _E4
	
	
};

} // namespace System12

#endif
