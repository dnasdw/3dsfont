#ifndef COMMON_FONT_TEXTURE_GLYPH_H_
#define COMMON_FONT_TEXTURE_GLYPH_H_

#include "utility.h"

#include MSC_PUSH_PACKED
struct FontTextureGlyph
{
	u8 CellWidth;
	u8 CellHeight;
	n8 BaselinePos;
	u8 MaxCharWidth;
	u32 SheetSize;
	u16 SheetNum;
	u16 SheetFormat;
	u16 SheetRow;
	u16 SheetLine;
	u16 SheetWidth;
	u16 SheetHeight;
	u32 SheetImage;
};
#include MSC_POP_PACKED

#endif	// COMMON_FONT_TEXTURE_GLYPH_H_
