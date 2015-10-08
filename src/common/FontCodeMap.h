#ifndef COMMON_FONT_CODE_MAP_H_
#define COMMON_FONT_CODE_MAP_H_

#include "utility.h"

#include MSC_PUSH_PACKED
struct FontCodeMap
{
	u16 CodeBegin;
	u16 CodeEnd;
	u16 MappingMethod;
	u16 Reserved;
	u32 PtrNext;
} GNUC_PACKED;
#include MSC_POP_PACKED

#endif	// COMMON_FONT_CODE_MAP_H_
