#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "bucket_t.h"

#define DEBUG_MODE 1
#define MEM_SIZE 2048
#define MIN_BLOCK 16
extern bucket_t * free_list;
extern bucket_t * busy_list;

/********************************************************************** 
* Purpose: Adds a block to a memory list.
*
* Precondition: 
*            <bucket> is the memory list to add the element to.
*            <size> is the size of the block.
*            <addr> is the actual offset of the block.
*
* Postcondition: 
*            <addr> is adapted to short offset
*            <bucket> now contains the entry for block(size, addr)
*
************************************************************************/
void Add(bucket_t * bucket, int size, int addr)
{
    int order = computeOrder(size);
    int used = bucket[order].m_count;
    bucket[order].m_offset[used] = addr >> 4;
    bucket[order].m_count = used + 1;
}

/********************************************************************** 
* Purpose: Splits a block of size <size> into 2 blocks of <size>/2
*
* Precondition: 
*           <size> is the size of a block we wish to split.
*
* Postcondition: 
*           Splits a block of size <size> if available. Returns the address of
*           one of the split blocks.
*
*           Error: Returns -1
*
*           RECURSIVE
************************************************************************/
intptr_t split(int size)
{
    int order = computeOrder(size);

    if(size > MEM_SIZE) return -1;

    int count = free_list[order].m_count;
    int offset;

    if(count == 0) // Split one of the size up
    {
        split(size << 1);
        count = free_list[order].m_count;
    }

    offset = (free_list[order].m_offset[count - 1]) << 4;

    free_list[order].m_count = count - 1;
    Add(free_list, size>>1, offset);
    Add(free_list, size>>1, offset + (size >> 1));
    return offset;
}

intptr_t Join(int order, int offsetA, int offsetB)
{
    int destCount = free_list[order + 1].m_count;
    int srcCount = free_list[order].m_count;

    // Set the lower of the two addresses
    free_list[order + 1].m_offset[destCount] = offsetA>offsetB?offsetB:offsetA;

    // Remove source offsetA and offsetB
}

void Move(bucket_t * dest, bucket_t * source, int order, int offset)
{
    int destCount = dest[order].m_count;
    int srcCount = source[order].m_count;

    // Insert at end of free_list
    dest[order].m_offset[destCount] = source[order].m_offset[offset];
    dest[order].m_count = destCount + 1;

    // Reorder source list
    if(offset < srcCount)
    {
        source[order].m_offset[offset] = source[order].m_offset[srcCount];
        source[order].m_count = srcCount - 1;
    }
}