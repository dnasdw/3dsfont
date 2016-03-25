#ifndef COMMON_UTILITY_H_
#define COMMON_UTILITY_H_

#define COMPILER_MSC  1
#define COMPILER_GNUC 2

#if defined(_MSC_VER)
#define _3DSFONT_COMPILER COMPILER_MSC
#else
#define _3DSFONT_COMPILER COMPILER_GNUC
#endif

#if defined(_MSC_VER)
#define _3DSFONT_COMPILER COMPILER_MSC
#define _3DSFONT_COMPILER_VERSION _MSC_VER
#else
#define _3DSFONT_COMPILER COMPILER_GNUC
#define _3DSFONT_COMPILER_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif

#if _3DSFONT_COMPILER == COMPILER_MSC
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <iconv.h>
#endif
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;

typedef int8_t n8;
typedef int16_t n16;
typedef int32_t n32;
typedef int64_t n64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#if _3DSFONT_COMPILER == COMPILER_MSC
#if _3DSFONT_COMPILER_VERSION >= 1900
typedef u16 Char16_t;
typedef std::basic_string<Char16_t> U16String;
#else
typedef char16_t Char16_t;
typedef u16string U16String;
#endif
#define MSC_PUSH_PACKED <pshpack1.h>
#define MSC_POP_PACKED <poppack.h>
#define GNUC_PACKED
#else
typedef char16_t Char16_t;
typedef u16string U16String;
#define MSC_PUSH_PACKED <stdlib.h>
#define MSC_POP_PACKED <stdlib.h>
#define GNUC_PACKED __attribute__((packed))
#endif

#define CONVERT_ENDIAN(n) (((n) >> 24 & 0xFF) | ((n) >> 8 & 0xFF00) | (((n) & 0xFF00) << 8) | (((n) & 0xFF) << 24))

#if _3DSFONT_COMPILER == COMPILER_MSC
U16String FSCPNToU16(const string& a_sString, u32 a_uCodePage)
{
	static U16String sConverted;
	sConverted.clear();
	int nLength = MultiByteToWideChar(a_uCodePage, 0, a_sString.c_str(), -1, nullptr, 0);
	Char16_t* pTemp = new Char16_t[nLength];
	MultiByteToWideChar(a_uCodePage, 0, a_sString.c_str(), -1, reinterpret_cast<LPWSTR>(pTemp), nLength);
	sConverted = pTemp;
	delete[] pTemp;
	return sConverted;
}

U16String FSCP932ToU16(const string& a_sString)
{
	return FSCPNToU16(a_sString, 932);
}

U16String FSCP1252ToU16(const string& a_sString)
{
	return FSCPNToU16(a_sString, 1252);
}
#else
template<typename TSrc, typename TDest>
TDest FSTToT(const TSrc& a_sString, const string& a_sSrcType, const string& a_sDestType)
{
	TDest sConverted;
	iconv_t cvt = iconv_open(a_sDestType.c_str(), a_sSrcType.c_str());
	if (cvt == reinterpret_cast<iconv_t>(-1))
	{
		return sConverted;
	}
	size_t nStringLeft = a_sString.size() * sizeof(typename TSrc::value_type);
	static const n32 c_nBufferSize = 1024;
	static const n32 c_nConvertBufferSize = c_nBufferSize - 4;
	char buffer[c_nBufferSize];
	do
	{
		typename TSrc::value_type* pString = const_cast<typename TSrc::value_type*>(a_sString.c_str());
		char* pBuffer = buffer;
		size_t nBufferLeft = c_nConvertBufferSize;
		n32 nError = iconv(cvt, reinterpret_cast<char**>(&pString), &nStringLeft, &pBuffer, &nBufferLeft);
		if (nError == 0 || (nError == static_cast<size_t>(-1) && errno == E2BIG))
		{
			*reinterpret_cast<typename TDest::value_type*>(buffer + c_nConvertBufferSize - nBufferLeft) = 0;
			sConverted += reinterpret_cast<typename TDest::value_type*>(buffer);
			if (nError == 0)
			{
				break;
			}
		}
		else
		{
			break;
		}
	} while (true);
	iconv_close(cvt);
	return sConverted;
}

U16String FSCP932ToU16(const string& a_sString)
{
	return FSTToT<string, U16String>(a_sString, "CP932", "UTF-16LE");
}

U16String FSCP1252ToU16(const string& a_sString)
{
	return FSTToT<string, U16String>(a_sString, "CP1252", "UTF-16LE");
}
#endif

#endif	// COMMON_UTILITY_H_
