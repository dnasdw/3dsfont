#include <ResourceFormat.h>

int UMain(int argc, UChar* argv[])
{
	if (argc != 3)
	{
		return 1;
	}
	vector<pair<pair<u16, u16>, n32>> vFontKerning;
	FILE* fp = UFopen(argv[1], USTR("rb"), false);
	if (fp == nullptr)
	{
		return 1;
	}
	fseek(fp, 0, SEEK_END);
	u32 uFontSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	u8* pFont = new u8[uFontSize];
	fread(pFont, 1, uFontSize, fp);
	fclose(fp);
	EEndianness eEndianness = kEndiannessLittleEndian;
	SBinaryFileHeader* pBinaryFileHeader = reinterpret_cast<SBinaryFileHeader*>(pFont);
	if (pBinaryFileHeader->Signature != kConstSignatureFONT && pBinaryFileHeader->Signature != kConstSignatureFONTA)
	{
		delete[] pFont;
		return 1;
	}
	if (pBinaryFileHeader->ByteOrder == 0xFFFE)
	{
		eEndianness = kEndiannessBigEndian;
	}
	if (eEndianness == kEndiannessBigEndian)
	{
		pBinaryFileHeader->ByteOrder = SDW_CONVERT_ENDIAN16(pBinaryFileHeader->ByteOrder);
		pBinaryFileHeader->HeaderSize = SDW_CONVERT_ENDIAN16(pBinaryFileHeader->HeaderSize);
		pBinaryFileHeader->Version = SDW_CONVERT_ENDIAN32(pBinaryFileHeader->Version);
		pBinaryFileHeader->FileSize = SDW_CONVERT_ENDIAN32(pBinaryFileHeader->FileSize);
		pBinaryFileHeader->DataBlocks = SDW_CONVERT_ENDIAN16(pBinaryFileHeader->DataBlocks);
		pBinaryFileHeader->Reserved = SDW_CONVERT_ENDIAN16(pBinaryFileHeader->Reserved);
	}
	u32 uOffset = pBinaryFileHeader->HeaderSize;
	while (uOffset < uFontSize && vFontKerning.empty())
	{
		SBinaryBlockHeader* pBinaryBlockHeader = reinterpret_cast<SBinaryBlockHeader*>(pFont + uOffset);
		if (eEndianness == kEndiannessBigEndian)
		{
			pBinaryBlockHeader->Size = SDW_CONVERT_ENDIAN32(pBinaryBlockHeader->Size);
		}
		switch (pBinaryBlockHeader->Kind)
		{
		case kConstSignatureFINF:
		case kConstSignatureCGLP:
		case kConstSignatureTGLP:
		case kConstSignatureCWDH:
		case kConstSignatureCMAP:
			break;
		case kConstSignatureKRNG:
			{
				SFontKerningTable_NX* pFontKerningTableTest = reinterpret_cast<SFontKerningTable_NX*>(pBinaryBlockHeader + 1);
				bool bNX = pFontKerningTableTest->Padding[0] == 0 && pFontKerningTableTest->Padding[1] == 0;
				if (!bNX)
				{
					SFontKerningTable_CTR_CAFE* pFontKerningTable = reinterpret_cast<SFontKerningTable_CTR_CAFE*>(pBinaryBlockHeader + 1);
					if (eEndianness == kEndiannessBigEndian)
					{
						pFontKerningTable->FirstWordNum = SDW_CONVERT_ENDIAN16(pFontKerningTable->FirstWordNum);
					}
					u32 uFirstWordCount = pFontKerningTable->FirstWordNum;
					for (u32 i = 0; i < uFirstWordCount; i++)
					{
						if (eEndianness == kEndiannessBigEndian)
						{
							pFontKerningTable->FirstTable[i].FirstWord = SDW_CONVERT_ENDIAN16(pFontKerningTable->FirstTable[i].FirstWord);
							pFontKerningTable->FirstTable[i].Offset = SDW_CONVERT_ENDIAN16(pFontKerningTable->FirstTable[i].Offset);
						}
						u16 uFirstWord = pFontKerningTable->FirstTable[i].FirstWord;
						u32 uKerningSecondTableOffset = pFontKerningTable->FirstTable[i].Offset;
						SKerningSecondTable_CTR_CAFE* pKerningSecondTable = reinterpret_cast<SKerningSecondTable_CTR_CAFE*>(reinterpret_cast<u8*>(pFontKerningTable) + uKerningSecondTableOffset);
						if (eEndianness == kEndiannessBigEndian)
						{
							pKerningSecondTable->SecondWordNum = SDW_CONVERT_ENDIAN16(pKerningSecondTable->SecondWordNum);
						}
						u32 uSecondWordCount = pKerningSecondTable->SecondWordNum;
						for (u32 j = 0; j < uSecondWordCount; j++)
						{
							if (eEndianness == kEndiannessBigEndian)
							{
								pKerningSecondTable->Elems[j].SecondWord = SDW_CONVERT_ENDIAN16(pKerningSecondTable->Elems[j].SecondWord);
								pKerningSecondTable->Elems[j].KerningValue = SDW_CONVERT_ENDIAN16(pKerningSecondTable->Elems[j].KerningValue);
							}
							u16 uSecondWord = pKerningSecondTable->Elems[j].SecondWord;
							n32 nKerningValue = pKerningSecondTable->Elems[j].KerningValue;
							vFontKerning.push_back(make_pair(make_pair(uFirstWord, uSecondWord), nKerningValue));
						}
					}
				}
				else
				{
					SFontKerningTable_NX* pFontKerningTable = reinterpret_cast<SFontKerningTable_NX*>(pBinaryBlockHeader + 1);
					if (eEndianness == kEndiannessBigEndian)
					{
						pFontKerningTable->FirstWordCount = SDW_CONVERT_ENDIAN16(pFontKerningTable->FirstWordCount);
					}
					u32 uFirstWordCount = pFontKerningTable->FirstWordCount;
					for (u32 i = 0; i < uFirstWordCount; i++)
					{
						if (eEndianness == kEndiannessBigEndian)
						{
							pFontKerningTable->FirstTable[i].FirstWord = SDW_CONVERT_ENDIAN32(pFontKerningTable->FirstTable[i].FirstWord);
							pFontKerningTable->FirstTable[i].Offset = SDW_CONVERT_ENDIAN32(pFontKerningTable->FirstTable[i].Offset);
						}
						u16 uFirstWord = pFontKerningTable->FirstTable[i].FirstWord & 0xFFFF;
						u32 uKerningSecondTableOffset = pFontKerningTable->FirstTable[i].Offset;
						SKerningSecondTable_NX* pKerningSecondTable = reinterpret_cast<SKerningSecondTable_NX*>(reinterpret_cast<u8*>(pFontKerningTable) + uKerningSecondTableOffset);
						if (eEndianness == kEndiannessBigEndian)
						{
							pKerningSecondTable->SecondWordCount = SDW_CONVERT_ENDIAN16(pKerningSecondTable->SecondWordCount);
						}
						u32 uSecondWordCount = pKerningSecondTable->SecondWordCount;
						for (u32 j = 0; j < uSecondWordCount; j++)
						{
							if (eEndianness == kEndiannessBigEndian)
							{
								pKerningSecondTable->Elems[j].SecondWord = SDW_CONVERT_ENDIAN32(pKerningSecondTable->Elems[j].SecondWord);
								pKerningSecondTable->Elems[j].KerningValue = SDW_CONVERT_ENDIAN16(pKerningSecondTable->Elems[j].KerningValue);
							}
							u16 uSecondWord = pKerningSecondTable->Elems[j].SecondWord;
							n32 nKerningValue = pKerningSecondTable->Elems[j].KerningValue;
							vFontKerning.push_back(make_pair(make_pair(uFirstWord, uSecondWord), nKerningValue));
						}
					}
				}
			}
			break;
		case kConstSignatureGLGR:
		case kConstSignatureHTGL:
			break;
		default:
			delete[] pFont;
			return 1;
		}
		uOffset += pBinaryBlockHeader->Size;
	}
	delete[] pFont;
	if (vFontKerning.empty())
	{
		return 0;
	}
	fp = UFopen(argv[2], USTR("wb"), false);
	if (fp == nullptr)
	{
		return 1;
	}
	for (vector<pair<pair<u16, u16>, n32>>::iterator it = vFontKerning.begin(); it != vFontKerning.end(); ++it)
	{
		pair<pair<u16, u16>, n32>& pKerning = *it;
		fwrite(&pKerning.first.first, 2, 1, fp);
		fwrite(&pKerning.first.second, 2, 1, fp);
		fwrite(&pKerning.second, 4, 1, fp);
	}
	fclose(fp);
	return 0;
}
