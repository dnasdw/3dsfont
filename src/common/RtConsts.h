#ifndef RT_CONSTS_H_
#define RT_CONSTS_H_

#include "utility.h"

struct RtConsts
{
	static const u16 InvalidGlyphIndex = 0xFFFF;
	static const u32 BinFileSigFONT = CONVERT_ENDIAN('CFNT');
	static const u32 BinFileSigFONTA = CONVERT_ENDIAN('CFNA');
	static const u32 BinBlockSigFINF = CONVERT_ENDIAN('FINF');
	static const u32 BinBlockSigCGLP = CONVERT_ENDIAN('CGLP');
	static const u32 BinBlockSigTGLP = CONVERT_ENDIAN('TGLP');
	static const u32 BinBlockSigCWDH = CONVERT_ENDIAN('CWDH');
	static const u32 BinBlockSigCMAP = CONVERT_ENDIAN('CMAP');
	static const u32 BinBlockSigGLGR = CONVERT_ENDIAN('GLGR');
	static const u32 BinBlockSigHTGL = CONVERT_ENDIAN('HTGL');
	static const u32 FontFileVersion;
	static u16 MakeVersion(u8 major, u8 minor)
	{
		return major << 8 | minor;
	}
	static u32 MakeVersion(u8 major, u8 minor, u8 micro, u8 binaryBugFix)
	{
		return major << 24 | minor << 16 | micro << 8 | binaryBugFix;
	}
};

const u32 RtConsts::FontFileVersion = RtConsts::MakeVersion(3, 0, 0, 0);

#endif	// RT_CONSTS_H_
