#include "alloc.hpp"

/// Allocator (Base)
Allocator::Allocator(size_t size, void* start)
    //: totalSize(size), startPtr(start), usedMem(0), numAllocs(0)
{
    startPtr    = start;
    allocSize   = size;

    usedMem     = 0;
    numAllocs   = 0;
}

Allocator::~Allocator()
{
    ASSERT(numAllocs == 0 && usedMem == 0);

    startPtr = nullptr;
    allocSize   = 0;
}

void* Allocator::getStart() const
{
    return startPtr;
}

size_t Allocator::getSize() const
{
    return allocSize;
}

size_t Allocator::getUsedMemory() const
{
    return usedMem;
}

size_t Allocator::getNumAllocations() const
{
    return numAllocs;
}


/// LinearAllocator
LinearAllocator::LinearAllocator(size_t size, void* start)
    : Allocator(size, start), marker(start)
{
    ASSERT(size > 0);
}

LinearAllocator::~LinearAllocator()
{
    marker   = nullptr;
}

void* LinearAllocator::allocate(size_t size, u8 alignment)
{
    ASSERT(size != 0);

    u8 adjustment =  pointer::alignForwardAdjustment(marker, alignment);

    if(usedMem + adjustment + size > allocSize)
        return nullptr;

    u8* aligned_address = (u8*)marker + adjustment;

    marker = (void*)(aligned_address + size);

    usedMem += size + adjustment;
    numAllocs++;

    return (void*)aligned_address;
}

void LinearAllocator::deallocate(void* p)
{
    ASSERT( false && "Use clear() instead" );
}

void LinearAllocator::clear()
{
    numAllocs     = 0;
    usedMem       = 0;

    marker    = startPtr;
}


/// StackAllocator
StackAllocator::StackAllocator(size_t size, void* start)
    : Allocator(size, start), marker(start)
{
    ASSERT(size > 0);

    #if ALLOC_DEBUG
    prev_marker     = nullptr;
    #endif
}

StackAllocator::~StackAllocator()
{
    #if ALLOC_DEBUG
    prev_marker     = nullptr;
    #endif

    marker          = nullptr;
}

void* StackAllocator::allocate(size_t size, u8 alignment)
{
    ASSERT(size != 0);

    u8 adjustment = pointer::alignForwardAdjustmentWithHeader(marker, alignment, sizeof(AllocationHeader));

    if(usedMem + adjustment + size > allocSize)
        return nullptr;

    void* aligned_address = pointer::add(marker, adjustment);

    //Add Allocation Header
    AllocationHeader* header = (AllocationHeader*)(pointer::subtract(aligned_address, sizeof(AllocationHeader)));

    header->adjustment   = adjustment;

    #if ALLOC_DEBUG
    header->prev_address = prev_marker;

    prev_marker          = aligned_address;
    #endif

    marker = pointer::add(aligned_address, size);

    usedMem += size + adjustment;
    numAllocs++;

    return aligned_address;
}

void StackAllocator::deallocate(void* p)
{
    ASSERT( p == prev_marker );

    //Access the AllocationHeader in the bytes before p
    AllocationHeader* header = (AllocationHeader*)(pointer::subtract(p, sizeof(AllocationHeader)));

    usedMem -= (uptr)marker - (uptr)p + header->adjustment;

    marker = pointer::subtract(p, header->adjustment);

    #if ALLOC_DEBUG
    prev_marker = header->prev_address;
    #endif

    numAllocs--;
}


/// FreeListAllocator
FreeListAllocator::FreeListAllocator(size_t size, void* start)
    : Allocator(size, start), free_blocks((FreeBlock*)start)
{
    ASSERT(size > sizeof(FreeBlock));

    free_blocks->size  = size;
    free_blocks->next = nullptr;
}

FreeListAllocator::~FreeListAllocator()
{
    free_blocks        = nullptr;
}

void* FreeListAllocator::allocate(size_t size, u8 alignment)
{
    ASSERT(size != 0 && alignment != 0);

    FreeBlock* prev_free_block = nullptr;
    FreeBlock* free_block     = free_blocks;

    while(free_block != nullptr)
    {
        //Calculate adjustment needed to keep object correctly aligned
        u8 adjustment = pointer::alignForwardAdjustmentWithHeader(free_block, alignment, sizeof(AllocationHeader));

        size_t total_size = size + adjustment;

        //If allocation doesn't fit in this FreeBlock, try the next
        if(free_block->size < total_size)
        {
            prev_free_block = free_block;
            free_block = free_block->next;
            continue;
        }

        static_assert(sizeof(AllocationHeader) >= sizeof(FreeBlock), "sizeof(AllocationHeader) < sizeof(FreeBlock)");

        //If allocations in the remaining memory will be impossible
        if(free_block->size - total_size <= sizeof(AllocationHeader))
        {
            //Increase allocation size instead of creating a new FreeBlock
            total_size = free_block->size;

            if(prev_free_block != nullptr)
                prev_free_block->next = free_block->next;
            else
                free_blocks = free_block->next;
        }
        else
        {
            //Else create a new FreeBlock containing remaining memory
            FreeBlock* next_block = (FreeBlock*)( pointer::add(free_block, total_size) );
            next_block->size = free_block->size - total_size;
            next_block->next = free_block->next;

            if(prev_free_block != nullptr)
                prev_free_block->next = next_block;
            else
                free_blocks = next_block;
        }

        uptr aligned_address = (uptr)free_block + adjustment;

        AllocationHeader* header = (AllocationHeader*)(aligned_address - sizeof(AllocationHeader));
        header->size             = total_size;
        header->adjustment       = adjustment;

        usedMem += total_size;
        numAllocs++;

        ASSERT(pointer::alignForwardAdjustment((void*)aligned_address, alignment) == 0);

        return (void*)aligned_address;
    }

    //ASSERT(false && "Couldn't find free block large enough!");

    return nullptr;
}

void FreeListAllocator::deallocate(void* p)
{
    ASSERT(p != nullptr);

    AllocationHeader* header = (AllocationHeader*)pointer::subtract(p, sizeof(AllocationHeader));

    uptr   block_start = reinterpret_cast<uptr>(p) - header->adjustment;
    size_t block_size  = header->size;
    uptr   block_end   = block_start + block_size;

    FreeBlock* prev_free_block = nullptr;
    FreeBlock* free_block = free_blocks;

    while(free_block != nullptr)
    {
        if( (uptr) free_block >= block_end )
            break;

        prev_free_block = free_block;
        free_block = free_block->next;
    }

    if(prev_free_block == nullptr)
    {
        prev_free_block = (FreeBlock*) block_start;
        prev_free_block->size = block_size;
        prev_free_block->next = free_blocks;

        free_blocks = prev_free_block;
    } else if((uptr) prev_free_block + prev_free_block->size == block_start)
    {
        prev_free_block->size += block_size;
    } else
    {
        FreeBlock* temp = (FreeBlock*) block_start;
        temp->size = block_size;
        temp->next = prev_free_block->next;
        prev_free_block->next = temp;

        prev_free_block = temp;
    }

    if( free_block != nullptr && (uptr) free_block == block_end)
    {
        prev_free_block->size += free_block->size;
        prev_free_block->next = free_block->next;
    }

    numAllocs--;
    usedMem -= block_size;
}


/// PoolAllocator
PoolAllocator::PoolAllocator(size_t objectSize, u8 objectAlignment, size_t size, void* mem)
    : Allocator(size, mem), objectSize(objectSize), objectAlignment(objectAlignment)
{
    ASSERT(objectSize >= sizeof(void*));

    //Calculate adjustment needed to keep object correctly aligned
    u8 adjustment = pointer::alignForwardAdjustment(mem, objectAlignment);

    freeList = (void**)pointer::add(mem, adjustment);

    size_t numObjects = (size-adjustment)/objectSize;

    void** p = freeList;

    //Initialize free blocks list
    for(size_t i = 0; i < numObjects-1; i++)
    {
        *p = pointer::add(p, objectSize );
        p = (void**) *p;
    }

    *p = nullptr;
}

PoolAllocator::~PoolAllocator()
{
    freeList = nullptr;
}

void* PoolAllocator::allocate(size_t size, u8 alignment)
{
    ASSERT(size == objectSize && alignment == objectAlignment);

    if(freeList == nullptr)
        return nullptr;

    void* p = freeList;

    freeList = (void**)(*freeList);

    usedMem += size;
    numAllocs++;

    return p;
}

void PoolAllocator::deallocate(void* p)
{
    *((void**)p) = freeList;

    freeList = (void**)p;

    usedMem -= objectSize;
    numAllocs--;
}


/// ProxyAllocator
ProxyAllocator::ProxyAllocator(Allocator& allocator)
    : Allocator(allocator.getSize(), allocator.getStart()), allocator(allocator)
{
}

ProxyAllocator::~ProxyAllocator()
{
}

void* ProxyAllocator::allocate(size_t size, u8 alignment)
{
    ASSERT(size != 0);
    numAllocs++;

    size_t mem = allocator.getUsedMemory();

    void* p = allocator.allocate(size, alignment);

    usedMem += allocator.getUsedMemory() - mem;

    return p;
}

void ProxyAllocator::deallocate(void* p)
{
    numAllocs--;

    size_t mem = allocator.getUsedMemory();

    allocator.deallocate(p);

    usedMem -= mem - allocator.getUsedMemory();
}
