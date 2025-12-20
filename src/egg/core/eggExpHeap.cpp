#include "types.h"
#include <egg/core/eggExpHeap.h>

namespace EGG {

ExpHeap::~ExpHeap()
{
	dispose();
	MEMDestroyExpHeap(mHeapHandle);
}

// issue with _savegpr_27_, unsure how to generate stmw r27, 0xc instead
ExpHeap* ExpHeap::create(void* p, u32 size, u16 idk)
{
	ExpHeap* heap = nullptr;

	u32 begin = ALIGN_NEXT((u32)p, 4);
	u32 end   = ALIGN_PREV((u32)p + size, 4);

	if (end > begin || end - begin < sizeof(MEMiHeapHead)) {
		return nullptr;
	}

	MEMiHeapHead* r27 = MEMCreateExpHeapEx((void*)(begin + sizeof(MEMiHeapHead) - 4), (void*)(end - sizeof(MEMiHeapHead) + 4));
	if (r27) {
		Heap* r31 = Heap::findContainHeap(p);

		heap              = new (p) ExpHeap(r27);
		heap->mHeapBuffer = (void*)begin;
		heap->mParentHeap = r31;

		if (Heap::sCreateCallback) {
			Heap::sCreateCallback(heap);
		}
	}

	// FORCE_DONT_INLINE;
	return heap;
}

// issue with _savegpr_27_, unsure how to generate stmw r27, 0xc instead
ExpHeap* ExpHeap::create(u32 size, Heap* parent, u16 idk)
{
	ExpHeap* heap = nullptr;

	if (!parent) {
		parent = Heap::sCurrentHeap;
	}
	if (size == 0xFFFFFFFF) {
		size = parent->getAllocatableSize();
	}
	void* pBlock = parent->alloc(size);
	if (pBlock) {

		heap = ExpHeap::create(pBlock, size, idk);

		if (!heap) {
			parent->free(pBlock);
		}
	}

	return heap;
}

} // namespace EGG
