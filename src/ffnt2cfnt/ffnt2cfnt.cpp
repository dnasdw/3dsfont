#include <utility.h>
#include <BinaryBlockHeader.h>
#include <BinaryFileHeader.h>
#include <FontInformation.h>
#include <FontTextureGlyph.h>
#include <RtConsts.h>

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		return 1;
	}
	FILE* fp = fopen(argv[1], "rb");
	if (fp == nullptr)
	{
		return 1;
	}
	fseek(fp, 0, SEEK_END);
	int nFontSize = ftell(fp);
	if (nFontSize < sizeof(BinaryFileHeader))
	{
		fclose(fp);
		return 1;
	}
	fseek(fp, 0, SEEK_SET);
	u8* pFont = new u8[nFontSize];
	fread(pFont, 1, nFontSize, fp);
	fclose(fp);
	BinaryFileHeader* pBinaryFileHeader = reinterpret_cast<BinaryFileHeader*>(pFont);
	if (pBinaryFileHeader->Signature != RtConsts::BinFileSigFONT + 3 && pBinaryFileHeader->Signature != RtConsts::BinFileSigFONTA + 3)
	{
		delete[] pFont;
		return 1;
	}
	int nOffset = pBinaryFileHeader->HeaderSize;
	while (true)
	{
		BinaryBlockHeader* pBinaryBlockHeader = reinterpret_cast<BinaryBlockHeader*>(pFont + nOffset);
		if (pBinaryBlockHeader->Kind == RtConsts::BinBlockSigFINF)
		{
			nOffset += sizeof(BinaryBlockHeader);
			FontInformation* pFontInformation = reinterpret_cast<FontInformation*>(pFont + nOffset);
			nOffset = pFontInformation->PtrWidth;
			if (nOffset != 0)
			{
				nOffset -= sizeof(BinaryBlockHeader);
				pBinaryBlockHeader = reinterpret_cast<BinaryBlockHeader*>(pFont + nOffset);
				if (pBinaryBlockHeader->Kind != RtConsts::BinBlockSigTGLP)
				{
					delete[] pFont;
					return 1;
				}
				nOffset += sizeof(BinaryBlockHeader);
				FontTextureGlyph* pFontTextureGlyph = reinterpret_cast<FontTextureGlyph*>(pFont + nOffset);
				n32 nTemp = pFontTextureGlyph->BaselinePos;
				pFontTextureGlyph->BaselinePos = pFontTextureGlyph->SheetNum & 0xFF;
				pFontTextureGlyph->SheetNum = nTemp &0xFFFF;
			}
			n64 nTemp = *reinterpret_cast<u32*>(&pFontInformation->FontType);
			pFontInformation->Linefeed = *reinterpret_cast<n16*>(&pFontInformation->DefaultWidth.Left) & 0xFF;
			pFontInformation->AlterCharIndex = *reinterpret_cast<u16*>(&pFontInformation->DefaultWidth.CharWidth);
			*reinterpret_cast<u32*>(&pFontInformation->DefaultWidth) = pFontInformation->PtrGlyph;
			pFontInformation->PtrGlyph = pFontInformation->PtrWidth;
			pFontInformation->PtrWidth = pFontInformation->PtrMap;
			pFontInformation->PtrMap = *reinterpret_cast<u32*>(&pFontInformation->Height);
			pFontInformation->Height = nTemp >> 8 & 0xFF;
			pFontInformation->Width = nTemp >> 16 & 0xFF;
			pFontInformation->Ascent = nTemp >> 24 & 0xFF;
			pFontInformation->Reserved = 0;
			break;
		}
		else if (pBinaryBlockHeader->Kind != RtConsts::BinBlockSigCGLP && pBinaryBlockHeader->Kind != RtConsts::BinBlockSigTGLP && pBinaryBlockHeader->Kind != RtConsts::BinBlockSigCWDH && pBinaryBlockHeader->Kind != RtConsts::BinBlockSigGLGR && pBinaryBlockHeader->Kind != RtConsts::BinBlockSigHTGL)
		{
			delete[] pFont;
			return 1;
		}
		else
		{
			nOffset += pBinaryBlockHeader->Size;
		}
	}
	pBinaryFileHeader->Signature -= 3;
	pBinaryFileHeader->Version = RtConsts::FontFileVersion;
	fp = fopen(argv[2], "wb");
	if (fp == nullptr)
	{
		delete[] pFont;
		return 1;
	}
	fwrite(pFont, 1, nFontSize, fp);
	fclose(fp);
	delete[] pFont;
	return 0;
}
