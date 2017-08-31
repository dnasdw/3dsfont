#include <sdw.h>

int UMain(int argc, UChar* argv[])
{
	if (argc < 3)
	{
		return 1;
	}
	set<Char16_t> sCharset;
	for (n32 i = 2; i < argc; i++)
	{
		FILE* fp = UFopen(argv[i], USTR("rb"), false);
		if (fp == nullptr)
		{
			return 1;
		}
		fseek(fp, 0, SEEK_END);
		u32 uTxtSize = ftell(fp);
		if (uTxtSize % 2 != 0)
		{
			fclose(fp);
			return 1;
		}
		uTxtSize /= 2;
		fseek(fp, 0, SEEK_SET);
		Char16_t* pTemp = new Char16_t[uTxtSize + 1];
		fread(pTemp, 2, uTxtSize, fp);
		fclose(fp);
		if (pTemp[0] != 0xFEFF)
		{
			delete[] pTemp;
			return 1;
		}
		pTemp[uTxtSize] = 0;
		U16String sTxt = pTemp + 1;
		delete[] pTemp;
		for (u32 j = 0; j < static_cast<u32>(sTxt.size()); j++)
		{
			Char16_t uUnicode = sTxt[j];
			if (uUnicode >= 0x20)
			{
				sCharset.insert(uUnicode);
			}
		}
	}
	FILE* fp = UFopen(argv[1], USTR("wb"), false);
	if (fp == nullptr)
	{
		return 1;
	}
	fwrite("\xFF\xFE", 2, 1, fp);
	n32 nIndex = 0;
	for (set<Char16_t>::iterator it = sCharset.begin(); it != sCharset.end(); ++it)
	{
		Char16_t uUnicode = *it;
		fwrite(&uUnicode, 2, 1, fp);
		if ((nIndex++ % 16) == 15)
		{
			fu16printf(fp, L"\r\n");
		}
	}
	fclose(fp);
	return 0;
}
