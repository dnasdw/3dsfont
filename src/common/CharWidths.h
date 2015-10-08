#ifndef COMMON_CHAR_WIDTHS_H_
#define COMMON_CHAR_WIDTHS_H_

#include "utility.h"

#include MSC_PUSH_PACKED
struct CharWidths
{
	n8 Left;
	u8 GlyphWidth;
	n8 CharWidth;
} GNUC_PACKED;
#include MSC_POP_PACKED

#endif	// COMMON_CHAR_WIDTHS_H_
