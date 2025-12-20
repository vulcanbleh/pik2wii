#include <egg/core/eggAllocator.h>
#include <egg/core/eggHeap.h>


namespace EGG {

Allocator::Allocator(Heap *pHeap, s32 align) : mHeap(pHeap), mAlign(align) {
    pHeap->initAllocator(this, align);
}

Allocator::~Allocator() {}

void *Allocator::alloc(u32 size) {
    return Heap::alloc(size, mAlign, mHeap);
}

void Allocator::free(void *block) {
    Heap::free(block, mHeap);
}

} // namespace EGG