#include <ResourceFormat.h>

Char16_t CcodeToU16Code(u8 a_uCharacterCode, u16 a_uCcode)
{
	if (a_uCcode < 0x20)
	{
		return 0;
	}
	switch (a_uCharacterCode)
	{
	case kCharacterCodeUnicode:
		return a_uCcode;
	case kCharacterCodeSJIS:
		{
			string sTemp;
			if (a_uCcode > 0xFF)
			{
				sTemp.append(1, a_uCcode >> 8 & 0xFF);
			}
			sTemp.append(1, a_uCcode & 0xFF);
			U16String sConverted = XToU16(sTemp, 932, "CP932");
			return sConverted[0];
		}
	case kCharacterCodeCP1252:
		{
			string sTemp;
			if (a_uCcode > 0xFF)
			{
				sTemp.append(1, a_uCcode >> 8 & 0xFF);
			}
			sTemp.append(1, a_uCcode & 0xFF);
			U16String sConverted = XToU16(sTemp, 1252, "CP1252");
			return sConverted[0];
		}
	}
	return 0;
}

int UMain(int argc, UChar* argv[])
{
	if (argc != 3)
	{
		return 1;
	}
	Char16_t uOrderedCode[0x10000] = {};
	n32 nLastIndex = -1;
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
	SBinaryFileHeader* pBinaryFileHeader = reinterpret_cast<SBinaryFileHeader*>(pFont);
	if (pBinaryFileHeader->Signature != kConstSignatureFONT && pBinaryFileHeader->Signature != kConstSignatureFONTA)
	{
		delete[] pFont;
		return 1;
	}
	if (pBinaryFileHeader->ByteOrder != 0xFEFF)
	{
		delete[] pFont;
		return 1;
	}
	SFontInformation* pFontInformation = nullptr;
	u32 uOffset = pBinaryFileHeader->HeaderSize;
	while (uOffset < uFontSize && pFontInformation == nullptr)
	{
		SBinaryBlockHeader* pBinaryBlockHeader = reinterpret_cast<SBinaryBlockHeader*>(pFont + uOffset);
		switch (pBinaryBlockHeader->Kind)
		{
		case kConstSignatureFINF:
			{
				pFontInformation = reinterpret_cast<SFontInformation*>(pBinaryBlockHeader + 1);
				if (pFontInformation->CharacterCode != kCharacterCodeUnicode && pFontInformation->CharacterCode != kCharacterCodeSJIS && pFontInformation->CharacterCode != kCharacterCodeCP1252)
				{
					delete[] pFont;
					return 1;
				}
				u32 uFontCodeMapOffset = pFontInformation->PMap;
				while (uFontCodeMapOffset != 0)
				{
					if (uFontCodeMapOffset >= uFontSize)
					{
						delete[] pFont;
						return 1;
					}
					uFontCodeMapOffset -= sizeof(SBinaryBlockHeader);
					pBinaryBlockHeader = reinterpret_cast<SBinaryBlockHeader*>(pFont + uFontCodeMapOffset);
					if (pBinaryBlockHeader->Kind != kConstSignatureCMAP)
					{
						delete[] pFont;
						return 1;
					}
					SFontCodeMap* pFontCodeMap = reinterpret_cast<SFontCodeMap*>(pBinaryBlockHeader + 1);
					u16* pMapInfo = pFontCodeMap->GetMapInfo();
					switch (pFontCodeMap->MappingMethod)
					{
					case kFontMapMethodDirect:
						{
							u16 uStartIndex = pMapInfo[0];
							for (n32 i = pFontCodeMap->CcodeBegin; i <= pFontCodeMap->CcodeEnd; i++)
							{
								uOrderedCode[uStartIndex + i - pFontCodeMap->CcodeBegin] = CcodeToU16Code(pFontInformation->CharacterCode, static_cast<u16>(i));
							}
							if (static_cast<n32>(uStartIndex + pFontCodeMap->CcodeEnd - pFontCodeMap->CcodeBegin) > nLastIndex)
							{
								nLastIndex = uStartIndex + pFontCodeMap->CcodeEnd - pFontCodeMap->CcodeBegin;
							}
						}
						break;
					case kFontMapMethodTable:
						{
							for (n32 i = pFontCodeMap->CcodeBegin; i <= pFontCodeMap->CcodeEnd; i++)
							{
								u16 uIndex = pMapInfo[i - pFontCodeMap->CcodeBegin];
								if (uIndex != kConstInvalidGlyphIndex)
								{
									uOrderedCode[uIndex] = CcodeToU16Code(pFontInformation->CharacterCode, static_cast<u16>(i));
									if (static_cast<n32>(uIndex) > nLastIndex)
									{
										nLastIndex = uIndex;
									}
								}
							}
						}
						break;
					case kFontMapMethodScan:
						{
							SCMapInfoScan* pCMapInfoScan = reinterpret_cast<SCMapInfoScan*>(pMapInfo);
							SCMapScanEntry* pCMapScanEntry = pCMapInfoScan->GetEntries();
							for (n32 i = 0; i < pCMapInfoScan->Num; i++)
							{
								uOrderedCode[pCMapScanEntry[i].Index] = CcodeToU16Code(pFontInformation->CharacterCode, pCMapScanEntry[i].Ccode);
								if (static_cast<n32>(pCMapScanEntry[i].Index) > nLastIndex)
								{
									nLastIndex = pCMapScanEntry[i].Index;
								}
							}
						}
						break;
					default:
						delete[] pFont;
						return 1;
					}
					uFontCodeMapOffset = pFontCodeMap->PNext;
				}
			}
			break;
		case kConstSignatureCGLP:
		case kConstSignatureTGLP:
		case kConstSignatureCWDH:
		case kConstSignatureCMAP:
		case kConstSignatureGLGR:
		case kConstSignatureHTGL:
			break;
		default:
			delete[] pFont;
			return 1;
		}
		uOffset += pBinaryBlockHeader->Size;
	}
	if (pFontInformation == nullptr)
	{
		delete[] pFont;
		return 1;
	}
	delete[] pFont;
	if (nLastIndex >= 0)
	{
		fp = UFopen(argv[2], USTR("wb"), false);
		if (fp == nullptr)
		{
			return 1;
		}
		fwrite("\xFF\xFE", 2, 1, fp);
		for (n32 i = 0; i <= nLastIndex; i++)
		{
			if (uOrderedCode[i] == 0)
			{
				fu16printf(fp, L" ");
			}
			else
			{
				fwrite(uOrderedCode + i, 2, 1, fp);
			}
			if ((i % 16) == 15)
			{
				fu16printf(fp, L"\r\n");
			}
		}
		fclose(fp);
	}
	return 0;
}
