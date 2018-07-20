#ifndef BFFNT_RESOURCEFORMAT_H_
#define BFFNT_RESOURCEFORMAT_H_

#include "../common/BinaryFileFormat.h"

enum EConst
{
	kConstSignatureFONT = SDW_CONVERT_ENDIAN32('FFNT'),
	kConstSignatureFONTA = SDW_CONVERT_ENDIAN32('FFNA'),
	kConstSignatureFCPX = SDW_CONVERT_ENDIAN32('FCPX'),
	kConstSignatureFINF = SDW_CONVERT_ENDIAN32('FINF'),
	kConstSignatureCGLP = SDW_CONVERT_ENDIAN32('CGLP'),
	kConstSignatureTGLP = SDW_CONVERT_ENDIAN32('TGLP'),
	kConstSignatureCWDH = SDW_CONVERT_ENDIAN32('CWDH'),
	kConstSignatureCMAP = SDW_CONVERT_ENDIAN32('CMAP'),
	kConstSignatureKRNG = SDW_CONVERT_ENDIAN32('KRNG'),
	kConstSignatureGLGR = SDW_CONVERT_ENDIAN32('GLGR'),
	kConstSignatureHTGL = SDW_CONVERT_ENDIAN32('HTGL'),

	kConstFontFileVersion_CTR_CAFE = MAKE_VERSION(4, 0, 0, 0),
	kConstFontFileVersion_NX = MAKE_VERSION(4, 1, 0, 0),

	kConstInvalidCharCode = 0xFFFF,
	kConstInvalidGlyphIndex = 0xFFFF
};

enum EFontType
{
	kFontTypeGlyph,
	kFontTypeTexture,
	kFontTypePackedTexture,

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

enum EFontSheetFormat_CTR
{
	kFontSheetFormat_CTRRGBA8,
	kFontSheetFormat_CTRRGB8,
	kFontSheetFormat_CTRRGB5A1,
	kFontSheetFormat_CTRRGB565,
	kFontSheetFormat_CTRRGBA4,
	kFontSheetFormat_CTRLA8,


	kFontSheetFormat_CTRA8 = 8,
	kFontSheetFormat_CTRLA4,

	kFontSheetFormat_CTRA4 = 11,



	kFontSheetFormat_CTRMask = 0x7FFF,
	kFontSheetFormat_CTRCompressedFlag = 0x8000
};

enum EFontSheetFormat_CAFE
{
	kFontSheetFormat_CAFERGBA8,
	kFontSheetFormat_CAFERGB8,
	kFontSheetFormat_CAFERGB5A1,
	kFontSheetFormat_CAFERGB565,
	kFontSheetFormat_CAFERGBA4,
	kFontSheetFormat_CAFELA8,
	kFontSheetFormat_CAFELA4,
	kFontSheetFormat_CAFEA4,
	kFontSheetFormat_CAFEA8 = 8,
	kFontSheetFormat_CAFEBC1,
	kFontSheetFormat_CAFEBC2,
	kFontSheetFormat_CAFEBC3,
	kFontSheetFormat_CAFEBC4 = 12,
	kFontSheetFormat_CAFEBC5,
	kFontSheetFormat_CAFERGBA8_SRGB,
	kFontSheetFormat_CAFEBC1_SRGB,
	kFontSheetFormat_CAFEBC2_SRGB,
	kFontSheetFormat_CAFEBC3_SRGB,
	kFontSheetFormat_CAFEQuantity,

	kFontSheetFormat_CAFEMake = 0x7FFF,
	kFontSheetFormat_CAFECompressedFlag = 0x8000
};

enum EFontSheetFormat_NX
{
	kFontSheetFormat_NXRGBA8,
	kFontSheetFormat_NXRGB8,
	kFontSheetFormat_NXRGB5A1,
	kFontSheetFormat_NXRGB565,
	kFontSheetFormat_NXRGBA4,
	kFontSheetFormat_NXLA8,
	kFontSheetFormat_NXLA4,
	kFontSheetFormat_NXA4,
	kFontSheetFormat_NXA8 = 8,
	kFontSheetFormat_NXBC1,
	kFontSheetFormat_NXBC2,
	kFontSheetFormat_NXBC3,
	kFontSheetFormat_NXBC4 = 12,
	kFontSheetFormat_NXBC5,
	kFontSheetFormat_NXRGBA8_SRGB,
	kFontSheetFormat_NXBC1_SRGB,
	kFontSheetFormat_NXBC2_SRGB,
	kFontSheetFormat_NXBC3_SRGB,
	kFontSheetFormat_NXBC7,
	kFontSheetFormat_NXBC7_SRGB,
	kFontSheetFormat_NXMax,

	kFontSheetFormat_NXMake = 0x3FFF,
	kFontSheetFormat_NXLinearFlag = 0x4000,
	kFontSheetFormat_NXCompressedFlag = 0x8000
};

#include SDW_MSC_PUSH_PACKED
struct SCharWidths
{
	n8 Left;
	u8 GlyphWidth;
	n8 CharWidth;
} SDW_GNUC_PACKED;

struct SCMapScanEntry_CTR_CAFE
{
	u16 Ccode;
	u16 Index;
} SDW_GNUC_PACKED;

struct SCMapScanEntry_NX
{
	u32 Code;
	u16 Index;
	u8 Padding[2];
} SDW_GNUC_PACKED;

struct SCMapInfoScan_CTR_CAFE
{
	u16 Num;
	SCMapScanEntry_CTR_CAFE Entries[1];
} SDW_GNUC_PACKED;

struct SCMapInfoScan_NX
{
	u16 Count;
	u8 Padding[2];
	SCMapScanEntry_NX Entries[1];
} SDW_GNUC_PACKED;

struct SKerningFirstTableElem_CTR_CAFE
{
	u16 FirstWord;
	u16 Offset;
} SDW_GNUC_PACKED;

struct SKerningFirstTableElem_NX
{
	u32 FirstWord;
	u32 Offset;
} SDW_GNUC_PACKED;

struct SKerningSecondTableElem_CTR_CAFE
{
	u16 SecondWord;
	n16 KerningValue;
} SDW_GNUC_PACKED;

struct SKerningSecondTableElem_NX
{
	u32 SecondWord;
	n16 KerningValue;
	u8 Padding[2];
} SDW_GNUC_PACKED;

struct SKerningSecondTable_CTR_CAFE
{
	u16 SecondWordNum;
	SKerningSecondTableElem_CTR_CAFE Elems[1];
} SDW_GNUC_PACKED;

struct SKerningSecondTable_NX
{
	u16 SecondWordCount;
	u8 Padding[2];
	SKerningSecondTableElem_NX Elems[1];
} SDW_GNUC_PACKED;

struct SFontTextureGlyph
{
	u8 CellWidth;
	u8 CellHeight;
	u8 SheetNum;	// NX: SheetCount
	u8 MaxCharWidth;
	u32 SheetSize;
	n16 BaselinePos;
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
	SCharWidths WidthTable[1];
} SDW_GNUC_PACKED;

struct SFontCodeMap_CTR_CAFE
{
	u16 CcodeBegin;
	u16 CcodeEnd;
	u16 MappingMethod;
	u16 Reserved;
	u32 PNext;
	u16 MapInfo[1];
} SDW_GNUC_PACKED;

struct SFontCodeMap_NX
{
	u32 CodeBegin;
	u32 CodeEnd;
	u16 MappingMethod;
	u16 Reserved;
	u32 PNext;
	u16 MapInfo[1];
} SDW_GNUC_PACKED;

struct SFontKerningTable_CTR_CAFE
{
	u16 FirstWordNum;
	SKerningFirstTableElem_CTR_CAFE FirstTable[1];
} SDW_GNUC_PACKED;

struct SFontKerningTable_NX
{
	u16 FirstWordCount;
	u8 Padding[2];
	SKerningFirstTableElem_NX FirstTable[1];
} SDW_GNUC_PACKED;

struct SFontInformation
{
	u8 FontType;
	u8 Height;
	u8 Width;
	u8 Ascent;
	n16 Linefeed;
	u16 AlterCharIndex;
	SCharWidths DefaultWidth;
	u8 CharacterCode;
	u32 PGlyph;
	u32 PWidth;
	u32 PMap;
} SDW_GNUC_PACKED;
#include SDW_MSC_POP_PACKED

#endif	// BFFNT_RESOURCEFORMAT_H_
