#ifndef _EGG_CORE_ALLOCATOR_H
#define _EGG_CORE_ALLOCATOR_H

#include <egg/egg_types.h>

namespace EGG {

class Heap;

struct unk_Allocator_base {
	u8 _00[0x10];
};

struct Allocator : public unk_Allocator_base {
	Allocator(Heap*, s32);

	virtual ~Allocator();

	virtual void* alloc(u32);
	virtual void free(void*);

	// _00-_10 = unk_Allocator_base
	// _10     = VTBL
	Heap* mHeap; // _14
	u32 mAlign;  // _18
};

} // namespace EGG

#endif
