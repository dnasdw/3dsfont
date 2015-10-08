#ifndef COMMON_BINARY_FILE_HEADER_H_
#define COMMON_BINARY_FILE_HEADER_H_

#include "utility.h"

#include MSC_PUSH_PACKED
struct BinaryFileHeader
{
	u32 Signature;
	u16 ByteOrder;
	u16 HeaderSize;
	u32 Version;
	u32 FileSize;
	u16 DataBlocks;
	u16 Reserved;
} GNUC_PACKED;
#include MSC_POP_PACKED

#endif	// COMMON_BINARY_FILE_HEADER_H_
