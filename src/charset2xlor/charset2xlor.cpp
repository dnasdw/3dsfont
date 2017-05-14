#include <sdw.h>

int UMain(int argc, UChar* argv[])
{
	if (argc != 3)
	{
		return 1;
	}
	FILE* fp = UFopen(argv[1], USTR("rb"), false);
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
	vector<Char16_t> vCharset;
	vCharset.reserve(sTxt.size());
	for (u32 i = 0; i < static_cast<u32>(sTxt.size()); i++)
	{
		Char16_t uUnicode = sTxt[i];
		if (uUnicode >= 0x20)
		{
			vCharset.push_back(uUnicode);
		}
	}
	string sTitle = UToU8(argv[2]);
	string::size_type uPos = sTitle.find_last_of("/\\");
	if (uPos != string::npos)
	{
		sTitle = sTitle.substr(uPos + 1);
	}
	fp = UFopen(argv[2], USTR("wb"), false);
	if (fp == nullptr)
	{
		return 1;
	}
	fprintf(fp, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n");
	fprintf(fp, "<!DOCTYPE letter-order SYSTEM \"letter-order.dtd\">\r\n");
	fprintf(fp, "\r\n");
	fprintf(fp, "<letter-order version=\"1.0\">\r\n");
	fprintf(fp, "	<head>\r\n");
	fprintf(fp, "		<create user=\"\" date=\"2014-12-09\" />\r\n");
	fprintf(fp, "		<title>%s</title>\r\n", sTitle.c_str());
	fprintf(fp, "		<comment></comment>\r\n");
	fprintf(fp, "	</head>\r\n");
	fprintf(fp, "\r\n");
	fprintf(fp, "	<body>\r\n");
	fprintf(fp, "		<area width=\"16\" />");
	fprintf(fp, "\r\n");
	fprintf(fp, "		<order>\r\n");
	bool bHasSp = false;
	n32 nIndex = 0;
	for (vector<Char16_t>::iterator it = vCharset.begin(); it != vCharset.end(); ++it)
	{
		Char16_t uUnicode = *it;
		if (nIndex % 16 == 0)
		{
			fprintf(fp, "		");
		}
		if (uUnicode == 0x20)
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
			fprintf(fp, "&#x%04X; ", uUnicode);
		}
		nIndex++;
		if (nIndex % 16 == 0)
		{
			fprintf(fp, "\r\n");
		}
	}
	fprintf(fp, "\r\n");
	fprintf(fp, "		</order>\r\n");
	fprintf(fp, "	</body>\r\n");
	fprintf(fp, "</letter-order>\r\n");
	fclose(fp);
	return 0;
}
