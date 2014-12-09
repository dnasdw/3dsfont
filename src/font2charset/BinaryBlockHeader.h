#ifndef BINARY_BLOCK_HEADER_H_
#define BINARY_BLOCK_HEADER_H_

#include "utility.h"

#include MSC_PUSH_PACKED
struct BinaryBlockHeader
{
	u32 Kind;
	u32 Size;
} GNUC_PACKED;
#include MSC_POP_PACKED

#endif	// BINARY_BLOCK_HEADER_H_
