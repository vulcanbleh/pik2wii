#ifndef EGG_CORE_ALLOCATOR_H
#define EGG_CORE_ALLOCATOR_H

#include <egg/egg_types.h>

#include <RevoSDK/mem.h>


namespace EGG {


class Heap;

class Allocator : public MEMAllocator {
public:
    Allocator(Heap* heap, s32 align);
	
    virtual ~Allocator();                    // _08
    virtual void* alloc(u32 size);			 // _0C
    virtual void free(void* pBlock); 		 // _10
	
	Heap *mHeap; 	// _04
    s32 mAlign; 	// _08
};

} // namespace EGG

#endif
