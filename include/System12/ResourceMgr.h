#ifndef _SYSTEM12_RESOURCE_MGR_H
#define _SYSTEM12_RESOURCE_MGR_H

#include <egg/core/eggSingleton.h>
#include <egg/core/eggHeap.h>

namespace System12 {

class SystemResourceMgr {
public:
	EGG_SINGLETON_DECL(SystemResourceMgr);
	
	SystemResourceMgr();
	
	virtual void construct(EGG::Heap*);
	
	// _00     = VTBL
	// _00-_10 = EGG::Disposer
	u32* _14; // _14
};


} // namespace System12

#endif
