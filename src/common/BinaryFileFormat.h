#ifndef COMMON_BINARYFILEFORMAT_H_
#define COMMON_BINARYFILEFORMAT_H_

#include <sdw.h>

#define MAKE_VERSION(major, minor, micro, binaryBugFix) \
	static_cast<u32>( \
		(static_cast<u8>(major) << 24) | (static_cast<u8>(micro) << 16) \
		| (static_cast<u8>(micro) << 8) | (static_cast<u8>(binaryBugFix)) \
	)

enum EEndianness
{
	kEndiannessBigEndian,
	kEndiannessLittleEndian
};

#include SDW_MSC_PUSH_PACKED
struct SBinaryFileHeader
{
	u32 Signature;
	u16 ByteOrder;
	u16 HeaderSize;
	u32 Version;
	u32 FileSize;
	u16 DataBlocks;
	u16 Reserved;
} SDW_GNUC_PACKED;

struct SBinaryBlockHeader
{
	u32 Kind;
	u32 Size;
} SDW_GNUC_PACKED;
#include SDW_MSC_POP_PACKED

#endif	// COMMON_BINARYFILEFORMAT_H_
