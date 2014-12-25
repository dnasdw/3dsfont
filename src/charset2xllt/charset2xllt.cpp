#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef int8_t n8;
typedef int16_t n16;
typedef int32_t n32;
typedef int64_t n64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

int main(int argc, char* argv[])
{
	u16 nLetter[0x10000] = {};
	nLetter[0x20] = 1;
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
	int charsetSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	u8* pCharset = new u8[charsetSize];
	fread(pCharset, 1, charsetSize, fp);
	fclose(fp);
	int nIndex = 0;
	if (charsetSize >= 3 && memcmp(pCharset, "\xEF\xBB\xBF", 3) == 0)
	{
		nIndex = 3;
	}
	for (int i = nIndex; i < charsetSize; i++)
	{
		u8* pUTF8 = pCharset + i;
		if (pUTF8[0] == 0 || pUTF8[0] > 0xEF)
		{
			delete[] pCharset;
			return 1;
		}
		else if (pUTF8[0] >= 0xE0)
		{
			nLetter[(pUTF8[0] << 12 & 0xF000) | (pUTF8[1] << 6 & 0xFC0) | (pUTF8[2] & 0x3F)] = 1;
			i += 2;
		}
		else if (pUTF8[0] >= 0xC0)
		{
			nLetter[(pUTF8[0] << 6 & 0x7C0) | (pUTF8[1] & 0x3F)] = 1;
			i++;
		}
		else if (pUTF8[0] >= 0x80)
		{
			delete[] pCharset;
			return 1;
		}
		else if (pUTF8[0] >= 0x20)
		{
			nLetter[pUTF8[0]] = 1;
		}
	}
	delete[] pCharset;
	const char* pName = strrchr(argv[2], '/');
	if (pName != nullptr)
	{
		pName++;
	}
	else
	{
		pName = argv[2];
	}
	const char* pName2 = strrchr(argv[2], '\\');
	if (pName2 > pName)
	{
		pName = pName2 + 1;
	}
	fp = fopen(argv[2], "wb");
	if (fp == nullptr)
	{
		return 1;
	}
	fprintf(fp, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n");
	fprintf(fp, "<!DOCTYPE letter-list SYSTEM \"letter-list.dtd\">\r\n");
	fprintf(fp, "\r\n");
	fprintf(fp, "<letter-list version=\"1.0\">\r\n");
	fprintf(fp, "	<head>\r\n");
	fprintf(fp, "		<create user=\"dnasdw\" date=\"2014-12-09\"/>\r\n");
	fprintf(fp, "		<title>%s</title>\r\n", pName);
	fprintf(fp, "		<comment></comment>\r\n");
	fprintf(fp, "	</head>\r\n");
	fprintf(fp, "\r\n");
	fprintf(fp, "	<body>\r\n");
	fprintf(fp, "		<letter>\r\n");
	int nCount = 0;
	for (int i = 0x20; i < 0x10000; i++)
	{
		if (nLetter[i] != 0)
		{
			if (nCount++ % 16 == 0)
			{
				fprintf(fp, "		");
			}
			if (i == 0x20)
			{
				fprintf(fp, "<sp/> ");
			}
			else
			{
				fprintf(fp, "&#x%04X; ", i);
			}
			if (nCount % 16 == 0)
			{
				fprintf(fp, "\r\n");
			}
		}
	}
	fprintf(fp, "\r\n");
	fprintf(fp, "		</letter>\r\n");
	fprintf(fp, "	</body>\r\n");
	fprintf(fp, "</letter-list>\r\n");
	fclose(fp);
	return 0;
}
