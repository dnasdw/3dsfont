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
	u16* pOrder = new u16[charsetSize];
	int nOrderSize = 0;
	for (int i = nIndex; i < charsetSize; i++)
	{
		u8* pUTF8 = pCharset + i;
		if (pUTF8[0] == 0 || pUTF8[0] > 0xEF)
		{
			delete[] pOrder;
			delete[] pCharset;
			return 1;
		}
		else if (pUTF8[0] >= 0xE0)
		{
			pOrder[nOrderSize++] = (pUTF8[0] << 12 & 0xF000) | (pUTF8[1] << 6 & 0xFC0) | (pUTF8[2] & 0x3F);
			i += 2;
		}
		else if (pUTF8[0] >= 0xC0)
		{
			pOrder[nOrderSize++] = (pUTF8[0] << 6 & 0x7C0) | (pUTF8[1] & 0x3F);
			i++;
		}
		else if (pUTF8[0] >= 0x80)
		{
			delete[] pOrder;
			delete[] pCharset;
			return 1;
		}
		else if (pUTF8[0] >= 0x20)
		{
			pOrder[nOrderSize++] = pUTF8[0];
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
		delete[] pOrder;
		return 1;
	}
	fprintf(fp, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");
	fprintf(fp, "<!DOCTYPE letter-order SYSTEM \"letter-order.dtd\">\r\n");
	fprintf(fp, "\r\n");
	fprintf(fp, "<letter-order version=\"1.0\">\r\n");
	fprintf(fp, "	<head>\r\n");
	fprintf(fp, "		<create user=\"dnasdw\" date=\"2014-12-09\"/>\r\n");
	fprintf(fp, "		<title>%s</title>\r\n", pName);
	fprintf(fp, "		<comment></comment>\r\n");
	fprintf(fp, "	</head>\r\n");
	fprintf(fp, "\r\n");
	fprintf(fp, "	<body>\r\n");
	fprintf(fp, "		<order>\r\n");
	bool bHasSp = false;
	for (int i = 0; i < nOrderSize; i += 16)
	{
		fprintf(fp, "		");
		for (int j = 0; j < 16 && i + j < nOrderSize; j++)
		{
			if (pOrder[i + j] == 0x20)
			{
				if (bHasSp)
				{
					fprintf(fp, "<null/> ");
				}
				else
				{
					fprintf(fp, "<sp/> ");
					bHasSp = true;
				}
			}
			else
			{
				fprintf(fp, "&#x%04X; ", pOrder[i + j]);
			}
		}
		fprintf(fp, "\r\n");
	}
	fprintf(fp, "\r\n");
	fprintf(fp, "		</order>\r\n");
	fprintf(fp, "	</body>\r\n");
	fprintf(fp, "</letter-order>\r\n");
	fclose(fp);
	delete[] pOrder;
	return 0;
}
