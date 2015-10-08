#ifndef COMMON_FONT_INFORMATION_H_
#define COMMON_FONT_INFORMATION_H_

#include "utility.h"
#include "CharWidths.h"

#include MSC_PUSH_PACKED
struct FontInformation
{
	u8 FontType;
	n8 Linefeed;
	u16 AlterCharIndex;
	CharWidths DefaultWidth;
	u8 Encoding;
	u32 PtrGlyph;
	u32 PtrWidth;
	u32 PtrMap;
	u8 Height;
	u8 Width;
	u8 Ascent;
	u8 Reserved;
} GNUC_PACKED;
#include MSC_POP_PACKED

#endif	// COMMON_FONT_INFORMATION_H_
