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
	SFontInformation* pFontInformation = nullptr;
	u32 uOffset = pBinaryFileHeader->HeaderSize;
	while (uOffset < uFontSize && pFontInformation == nullptr)
	{
		SBinaryBlockHeader* pBinaryBlockHeader = reinterpret_cast<SBinaryBlockHeader*>(pFont + uOffset);
		if (eEndianness == kEndiannessBigEndian)
		{
			pBinaryBlockHeader->Size = SDW_CONVERT_ENDIAN32(pBinaryBlockHeader->Size);
		}
		switch (pBinaryBlockHeader->Kind)
		{
		case kConstSignatureFINF:
			{
				pFontInformation = reinterpret_cast<SFontInformation*>(pBinaryBlockHeader + 1);
				if (eEndianness == kEndiannessBigEndian)
				{
					pFontInformation->Linefeed = SDW_CONVERT_ENDIAN16(pFontInformation->Linefeed);
					pFontInformation->AlterCharIndex = SDW_CONVERT_ENDIAN16(pFontInformation->AlterCharIndex);
					pFontInformation->PGlyph = SDW_CONVERT_ENDIAN32(pFontInformation->PGlyph);
					pFontInformation->PWidth = SDW_CONVERT_ENDIAN32(pFontInformation->PWidth);
					pFontInformation->PMap = SDW_CONVERT_ENDIAN32(pFontInformation->PMap);
				}
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
					if (eEndianness == kEndiannessBigEndian)
					{
						pBinaryBlockHeader->Size = SDW_CONVERT_ENDIAN32(pBinaryBlockHeader->Size);
					}
					if (pBinaryBlockHeader->Kind != kConstSignatureCMAP)
					{
						delete[] pFont;
						return 1;
					}
					SFontCodeMap_CTR_CAFE* pFontCodeMapTest = reinterpret_cast<SFontCodeMap_CTR_CAFE*>(pBinaryBlockHeader + 1);
					bool bNX = pFontCodeMapTest->CcodeEnd == 0;
					if (!bNX)
					{
						SFontCodeMap_CTR_CAFE* pFontCodeMap = reinterpret_cast<SFontCodeMap_CTR_CAFE*>(pBinaryBlockHeader + 1);
						if (eEndianness == kEndiannessBigEndian)
						{
							pFontCodeMap->CcodeBegin = SDW_CONVERT_ENDIAN16(pFontCodeMap->CcodeBegin);
							pFontCodeMap->CcodeEnd = SDW_CONVERT_ENDIAN16(pFontCodeMap->CcodeEnd);
							pFontCodeMap->MappingMethod = SDW_CONVERT_ENDIAN16(pFontCodeMap->MappingMethod);
							pFontCodeMap->Reserved = SDW_CONVERT_ENDIAN16(pFontCodeMap->Reserved);
							pFontCodeMap->PNext = SDW_CONVERT_ENDIAN32(pFontCodeMap->PNext);
						}
						u16* pMapInfo = pFontCodeMap->MapInfo;
						switch (pFontCodeMap->MappingMethod)
						{
						case kFontMapMethodDirect:
							{
								u16 uStartIndex = pMapInfo[0];
								if (eEndianness == kEndiannessBigEndian)
								{
									uStartIndex = SDW_CONVERT_ENDIAN16(uStartIndex);
								}
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
									if (eEndianness == kEndiannessBigEndian)
									{
										uIndex = SDW_CONVERT_ENDIAN16(uIndex);
									}
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
								SCMapInfoScan_CTR_CAFE* pCMapInfoScan = reinterpret_cast<SCMapInfoScan_CTR_CAFE*>(pMapInfo);
								SCMapScanEntry_CTR_CAFE* pCMapScanEntry = pCMapInfoScan->Entries;
								if (eEndianness == kEndiannessBigEndian)
								{
									pCMapInfoScan->Num = SDW_CONVERT_ENDIAN16(pCMapInfoScan->Num);
									for (n32 i = 0; i < pCMapInfoScan->Num; i++)
									{
										pCMapScanEntry[i].Ccode = SDW_CONVERT_ENDIAN16(pCMapScanEntry[i].Ccode);
										pCMapScanEntry[i].Index = SDW_CONVERT_ENDIAN16(pCMapScanEntry[i].Index);
									}
								}
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
					else
					{
						SFontCodeMap_NX* pFontCodeMap = reinterpret_cast<SFontCodeMap_NX*>(pBinaryBlockHeader + 1);
						if (eEndianness == kEndiannessBigEndian)
						{
							pFontCodeMap->CodeBegin = SDW_CONVERT_ENDIAN32(pFontCodeMap->CodeBegin);
							pFontCodeMap->CodeEnd = SDW_CONVERT_ENDIAN32(pFontCodeMap->CodeEnd);
							pFontCodeMap->MappingMethod = SDW_CONVERT_ENDIAN16(pFontCodeMap->MappingMethod);
							pFontCodeMap->Reserved = SDW_CONVERT_ENDIAN16(pFontCodeMap->Reserved);
							pFontCodeMap->PNext = SDW_CONVERT_ENDIAN32(pFontCodeMap->PNext);
						}
						u16* pMapInfo = pFontCodeMap->MapInfo;
						switch (pFontCodeMap->MappingMethod)
						{
						case kFontMapMethodDirect:
							{
								u16 uStartIndex = pMapInfo[0];
								if (eEndianness == kEndiannessBigEndian)
								{
									uStartIndex = SDW_CONVERT_ENDIAN16(uStartIndex);
								}
								for (n32 i = pFontCodeMap->CodeBegin; i <= static_cast<n32>(pFontCodeMap->CodeEnd); i++)
								{
									uOrderedCode[uStartIndex + i - pFontCodeMap->CodeBegin] = CcodeToU16Code(pFontInformation->CharacterCode, static_cast<u16>(i));
								}
								if (static_cast<n32>(uStartIndex + pFontCodeMap->CodeEnd - pFontCodeMap->CodeBegin) > nLastIndex)
								{
									nLastIndex = uStartIndex + pFontCodeMap->CodeEnd - pFontCodeMap->CodeBegin;
								}
							}
							break;
						case kFontMapMethodTable:
							{
								for (n32 i = pFontCodeMap->CodeBegin; i <= static_cast<n32>(pFontCodeMap->CodeEnd); i++)
								{
									u16 uIndex = pMapInfo[i - pFontCodeMap->CodeBegin];
									if (eEndianness == kEndiannessBigEndian)
									{
										uIndex = SDW_CONVERT_ENDIAN16(uIndex);
									}
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
								SCMapInfoScan_NX* pCMapInfoScan = reinterpret_cast<SCMapInfoScan_NX*>(pMapInfo);
								SCMapScanEntry_NX* pCMapScanEntry = pCMapInfoScan->Entries;
								if (eEndianness == kEndiannessBigEndian)
								{
									pCMapInfoScan->Count = SDW_CONVERT_ENDIAN16(pCMapInfoScan->Count);
									for (n32 i = 0; i < pCMapInfoScan->Count; i++)
									{
										pCMapScanEntry[i].Code = SDW_CONVERT_ENDIAN32(pCMapScanEntry[i].Code);
										pCMapScanEntry[i].Index = SDW_CONVERT_ENDIAN16(pCMapScanEntry[i].Index);
									}
								}
								for (n32 i = 0; i < pCMapInfoScan->Count; i++)
								{
									uOrderedCode[pCMapScanEntry[i].Index] = CcodeToU16Code(pFontInformation->CharacterCode, pCMapScanEntry[i].Code & 0xFFFF);
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
