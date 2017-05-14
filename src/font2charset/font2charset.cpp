#include <utility.h>
#include <BinaryBlockHeader.h>
#include <BinaryFileHeader.h>
#include <CMapInfoScan.h>
#include <CMapScanEntry.h>
#include <FontCodeMap.h>
#include <FontInformation.h>
#include <RtConsts.h>

u16 FCCP932ToU16(int a_nCode)
{
	string sTemp;
	if (a_nCode > 0xFF)
	{
		sTemp.append(1, a_nCode >> 8 & 0xFF);
	}
	sTemp.append(1, a_nCode & 0xFF);
	U16String sConverted = FSCP932ToU16(sTemp);
	return sConverted[0];
}

u16 FCCP1252ToU16(int a_nCode)
{
	string sTemp;
	if (a_nCode > 0xFF)
	{
		sTemp.append(1, a_nCode >> 8 & 0xFF);
	}
	sTemp.append(1, a_nCode & 0xFF);
	U16String sConverted = FSCP1252ToU16(sTemp);
	return sConverted[0];
}

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
						switch (pFontInformation->Encoding)
						{
						case 2:
							nIndexedCode[uStartIndex + i - pFontCodeMap->CodeBegin] = FCCP932ToU16(i);
							break;
						case 3:
							nIndexedCode[uStartIndex + i - pFontCodeMap->CodeBegin] = FCCP1252ToU16(i);
							break;
						default:
							nIndexedCode[uStartIndex + i - pFontCodeMap->CodeBegin] = i;
							break;
						}
						if (i < 0x20)
						{
							nIndexedCode[uStartIndex + i - pFontCodeMap->CodeBegin] = 0;
						}
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
							switch (pFontInformation->Encoding)
							{
							case 2:
								nIndexedCode[nIndex] = FCCP932ToU16(i);
								break;
							case 3:
								nIndexedCode[nIndex] = FCCP1252ToU16(i);
								break;
							default:
								nIndexedCode[nIndex] = i;
								break;
							}
							if (i < 0x20)
							{
								nIndexedCode[nIndex] = 0;
							}
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
						switch (pFontInformation->Encoding)
						{
						case 2:
							nIndexedCode[pCMapScanEntry[i].Index] = FCCP932ToU16(pCMapScanEntry[i].Code);
							break;
						case 3:
							nIndexedCode[pCMapScanEntry[i].Index] = FCCP1252ToU16(pCMapScanEntry[i].Code);
							break;
						default:
							nIndexedCode[pCMapScanEntry[i].Index] = pCMapScanEntry[i].Code;
							break;
						}
						if (pCMapScanEntry[i].Code < 0x20)
						{
							nIndexedCode[pCMapScanEntry[i].Index] = 0;
						}
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
	fwrite("\xFF\xFE", 2, 1, fp);
	for (int i = 0; i <= nLastIndex; i++)
	{
		if (nIndexedCode[i] == 0)
		{
			static u16 uSpace = 0x20;
			fwrite(&uSpace, 2, 1, fp);
		}
		else
		{
			fwrite(nIndexedCode + i, 2, 1, fp);
		}
		if ((i & 0xF) == 0xF)
		{
			u32 uCRLF = CONVERT_ENDIAN(0x0D000A00);
			fwrite(&uCRLF, 4, 1, fp);
		}
	}
	fclose(fp);
	return 0;
}
