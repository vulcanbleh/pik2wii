#ifndef _SYSTEM12_WP_TAGPROCESSOR_H
#define _SYSTEM12_WP_TAGPROCESSOR_H

#include <nw4r/ut.h>

namespace System12 {
	
struct WPMsgCallbackImpl;

class WPTagProcessor : public nw4r::ut::TagProcessorBase<wchar_t> {
public:
	WPTagProcessor();
	
	
	~WPTagProcessor();
	virtual Operation Process(u16 ch, nw4r::ut::TagProcessorBase<wchar_t>::ContextType*);               		// _0C
	virtual Operation CalcRect(nw4r::ut::Rect* pRect, u16, nw4r::ut::TagProcessorBase<wchar_t>::ContextType*); 	// _10
	virtual Operation RuntimeProcess(nw4r::ut::Rect*, u16, nw4r::ut::PrintContext<wchar_t>*);					// _14

	
	int CalcStrLen(const wchar_t*, int);
	int CalcStrLenAppointingNumWord(const wchar_t*, int, int*, int);
	int PreProcessWithNumWords(const wchar_t*, int, wchar_t*, int*, int*, int);
	
	static void ProcessTagFontColor(nw4r::ut::Rect*, nw4r::ut::PrintContext<wchar_t>*, u8, void*);
	static void ProcessTagFontSize(nw4r::ut::Rect*, nw4r::ut::PrintContext<wchar_t>*, u8, void*);
	static void ProcessTagRuby(nw4r::ut::Rect*, nw4r::ut::PrintContext<wchar_t>*, u8, void*);
	
	// unused/inlined:
	void SetMsgCallback(System12::WPMsgCallbackImpl);
	
private:	
	// _00     = VTBL
	// _00-_04 = nw4r::ut::TagProcessorBase<wchar_t>
	f32 _04;
	int _08;	
};

} // namespace System12

#endif
