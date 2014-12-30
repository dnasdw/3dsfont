#include "utility.h"
#include "BinaryBlockHeader.h"
#include "BinaryFileHeader.h"
#include "CMapInfoScan.h"
#include "CMapScanEntry.h"
#include "FontCodeMap.h"
#include "FontInformation.h"
#include "RtConsts.h"

int main(int argc, char* argv[])
{
	u16 nIndexedCode[0x10000] = {};
	int nLastIndex = 0;
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
	if (pBinaryFileHeader->Signature != RtConsts::BinFileSigFONT && pBinaryFileHeader->Signature != RtConsts::BinFileSigFONTA)
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
			nOffset = pFontInformation->PtrMap;
			while (nOffset != 0)
			{
				nOffset -= sizeof(BinaryBlockHeader);
				pBinaryBlockHeader = reinterpret_cast<BinaryBlockHeader*>(pFont + nOffset);
				if (pBinaryBlockHeader->Kind != RtConsts::BinBlockSigCMAP)
				{
					delete[] pFont;
					return 1;
				}
				nOffset += sizeof(BinaryBlockHeader);
				FontCodeMap* pFontCodeMap = reinterpret_cast<FontCodeMap*>(pFont + nOffset);
				nOffset += sizeof(FontCodeMap);
				if (pFontCodeMap->MappingMethod == 0)
				{
					u16 uStartIndex = *reinterpret_cast<u16*>(pFont + nOffset);
					for (int i = pFontCodeMap->CodeBegin; i <= pFontCodeMap->CodeEnd; i++)
					{
						nIndexedCode[uStartIndex + i - pFontCodeMap->CodeBegin] = i;
					}
					if (uStartIndex + pFontCodeMap->CodeEnd - pFontCodeMap->CodeBegin > nLastIndex)
					{
						nLastIndex = uStartIndex + pFontCodeMap->CodeEnd - pFontCodeMap->CodeBegin;
					}
				}
				else if (pFontCodeMap->MappingMethod == 1)
				{
					u16* pIndex = reinterpret_cast<u16*>(pFont + nOffset);
					for (int i = pFontCodeMap->CodeBegin; i <= pFontCodeMap->CodeEnd; i++)
					{
						int nIndex = *pIndex++;
						if (nIndex != RtConsts::InvalidGlyphIndex)
						{
							nIndexedCode[nIndex] = i;
							if (nIndex > nLastIndex)
							{
								nLastIndex = nIndex;
							}
						}
					}
				}
				else if (pFontCodeMap->MappingMethod == 2)
				{
					CMapInfoScan* pCMapInfoScan = reinterpret_cast<CMapInfoScan*>(pFont + nOffset);
					nOffset += sizeof(CMapInfoScan);
					CMapScanEntry* pCMapScanEntry = reinterpret_cast<CMapScanEntry*>(pFont + nOffset);
					for (int i = 0; i < pCMapInfoScan->Num; i++)
					{
						nIndexedCode[pCMapScanEntry[i].Index] = pCMapScanEntry[i].Code;
						if (pCMapScanEntry[i].Index > nLastIndex)
						{
							nLastIndex = pCMapScanEntry[i].Index;
						}
					}
				}
				else
				{
					delete[] pFont;
					return 1;
				}
				nOffset = pFontCodeMap->PtrNext;
			}
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
	delete[] pFont;
	fp = fopen(argv[2], "wb");
	if (fp == nullptr)
	{
		return 1;
	}
	fwrite("\xEF\xBB\xBF", 3, 1, fp);
	for (int i = 0; i <= nLastIndex; i++)
	{
		if (nIndexedCode[i] == 0)
		{
			fprintf(fp, " ");
		}
		else if (nIndexedCode[i] < 0x80)
		{
			fprintf(fp, "%c", nIndexedCode[i]);
		}
		else if (nIndexedCode[i] < 0x800)
		{
			u8 uUTF8[2] = {};
			uUTF8[0] = 0xC0 | (nIndexedCode[i] >> 6 & 0x1F);
			uUTF8[1] = 0x80 | (nIndexedCode[i] & 0x3F);
			fwrite(uUTF8, 2, 1, fp);
		}
		else
		{
			u8 uUTF8[3] = {};
			uUTF8[0] = 0xE0 | (nIndexedCode[i] >> 12 & 0xF);
			uUTF8[1] = 0x80 | (nIndexedCode[i] >> 6 & 0x3F);
			uUTF8[2] = 0x80 | (nIndexedCode[i] & 0x3F);
			fwrite(uUTF8, 3, 1, fp);
		}
		if ((i & 0xF) == 0xF)
		{
			fprintf(fp, "\r\n");
		}
	}
	fclose(fp);
	return 0;
}
