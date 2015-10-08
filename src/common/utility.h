#ifndef COMMON_UTILITY_H_
#define COMMON_UTILITY_H_

#define COMPILER_MSC  1
#define COMPILER_GNUC 2

#if defined(_MSC_VER)
#define _3DSFONT_COMPILER COMPILER_MSC
#else
#define _3DSFONT_COMPILER COMPILER_GNUC
#endif

#if _3DSFONT_COMPILER == COMPILER_MSC
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef int8_t n8;
typedef int16_t n16;
typedef int32_t n32;
typedef int64_t n64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#if _3DSFONT_COMPILER == COMPILER_MSC
#define MSC_PUSH_PACKED <pshpack1.h>
#define MSC_POP_PACKED <poppack.h>
#define GNUC_PACKED
#else
#define MSC_PUSH_PACKED <stdlib.h>
#define MSC_POP_PACKED <stdlib.h>
#define GNUC_PACKED __attribute__((packed))
#endif

#define CONVERT_ENDIAN(n) (((n) >> 24 & 0xFF) | ((n) >> 8 & 0xFF00) | (((n) & 0xFF00) << 8) | (((n) & 0xFF) << 24))

#endif	// COMMON_UTILITY_H_
