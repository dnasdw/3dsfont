#ifndef BCFNT_RESOURCEFORMAT_H_
#define BCFNT_RESOURCEFORMAT_H_

#include "../common/BinaryFileFormat.h"

enum EConst
{
	kConstSignatureFONT = SDW_CONVERT_ENDIAN32('CFNT'),
	kConstSignatureFONTA = SDW_CONVERT_ENDIAN32('CFNA'),
	kConstSignatureFINF = SDW_CONVERT_ENDIAN32('FINF'),
	kConstSignatureCGLP = SDW_CONVERT_ENDIAN32('CGLP'),
	kConstSignatureTGLP = SDW_CONVERT_ENDIAN32('TGLP'),
	kConstSignatureCWDH = SDW_CONVERT_ENDIAN32('CWDH'),
	kConstSignatureCMAP = SDW_CONVERT_ENDIAN32('CMAP'),
	kConstSignatureGLGR = SDW_CONVERT_ENDIAN32('GLGR'),
	kConstSignatureHTGL = SDW_CONVERT_ENDIAN32('HTGL'),

	kConstFontFileVersion = MAKE_VERSION(3, 0, 0, 0),

	kConstInvalidCharCode = 0xFFFF,
	kConstInvalidGlyphIndex = 0xFFFF
};

enum EFontType
{
	kFontTypeGlyph,
	kFontTypeTexture,

	kFontTypeMax
};

enum ECharacterCode
{
	kCharacterCodeUnicode = 1,
	kCharacterCodeSJIS,
	kCharacterCodeCP1252,

	kCharacterCodeMax
};

enum EFontMapMethod
{
	kFontMapMethodDirect,
	kFontMapMethodTable,
	kFontMapMethodScan,

	kFontMapMethodMax
};

enum EFontSheetFormat
{
	kFontSheetFormatRGBA8,
	kFontSheetFormatRGB8,
	kFontSheetFormatRGB5A1,
	kFontSheetFormatRGB565,
	kFontSheetFormatRGBA4,
	kFontSheetFormatLA8,


	kFontSheetFormatA8 = 8,
	kFontSheetFormatLA4,

	kFontSheetFormatA4 = 11,



	kFontSheetFormatMask = 0x7FFF,
	kFontSheetFormatCompressedFlag = 0x8000
};

#include SDW_MSC_PUSH_PACKED
struct SCharWidths
{
	n8 Left;
	u8 GlyphWidth;
	n8 CharWidth;
} SDW_GNUC_PACKED;

struct SCMapScanEntry
{
	u16 Ccode;
	u16 Index;
} SDW_GNUC_PACKED;

struct SCMapInfoScan
{
	u16 Num;
	SCMapScanEntry* GetEntries() const
	{
		return reinterpret_cast<SCMapScanEntry*>(const_cast<SCMapInfoScan*>(this) + 1);
	}
} SDW_GNUC_PACKED;

struct SFontTextureGlyph
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
} SDW_GNUC_PACKED;

struct SFontWidth
{
	u16 IndexBegin;
	u16 IndexEnd;
	u32 PNext;
	SCharWidths* GetWidthTable() const
	{
		return reinterpret_cast<SCharWidths*>(const_cast<SFontWidth*>(this) + 1);
	}
} SDW_GNUC_PACKED;

struct SFontCodeMap
{
	u16 CcodeBegin;
	u16 CcodeEnd;
	u16 MappingMethod;
	u16 Reserved;
	u32 PNext;
	u16* GetMapInfo() const
	{
		return reinterpret_cast<u16*>(const_cast<SFontCodeMap*>(this) + 1);
	}
} SDW_GNUC_PACKED;

struct SFontInformation
{
	u8 FontType;
	n8 Linefeed;
	u16 AlterCharIndex;
	SCharWidths DefaultWidth;
	u8 CharacterCode;
	u32 PGlyph;
	u32 PWidth;
	u32 PMap;
	u8 Height;
	u8 Width;
	u8 Ascent;
	u8 Padding[1];
} SDW_GNUC_PACKED;
#include SDW_MSC_POP_PACKED

#endif	// BCFNT_RESOURCEFORMAT_H_
