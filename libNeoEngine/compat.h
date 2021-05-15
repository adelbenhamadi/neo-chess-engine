#pragma once

#if defined(_MSC_VER)
// Disable some  MSVC warnings

#pragma warning(disable: 26812) // The enum type ... is unscoped.Prefer 'enum class' over 'enum'
#pragma warning(disable: 26451) // Arithmetic overflow TO_REMOVE
#pragma warning(disable: 4127) // Conditional expression is constant
#pragma warning(disable: 4146) // Unary minus operator applied to unsigned type
#pragma warning(disable: 4800) // Forcing value to bool 'true' or 'false'
#endif


#if defined(__GNUC__ ) && (__GNUC__ < 9 || (__GNUC__ == 9 && __GNUC_MINOR__ <= 2)) && defined(_WIN32) && !defined(__clang__)
#define ALIGNAS_ON_STACK_VARIABLES_BROKEN
#endif

#if !_WIN64
#define NO_POPCNT
#elif defined(NO_POPCNT)
#undefine NO_POPCNT
#endif

#if (defined(__INTEL_COMPILER) || defined(_MSC_VER))
#ifndef NO_POPCNT
#include <nmmintrin.h> // Intel and Microsoft header for _mm_popcnt_u64()
#define __builtin_popcountll _mm_popcnt_u64
#endif //NOT NO_POPCNT
#endif

#if defined(_WIN64) && defined(_MSC_VER) // No Makefile used
#  include <intrin.h> // Microsoft header for _BitScanForward64()
#  define IS_64BIT
#endif


#if defined(USE_PEXT)
#  include <immintrin.h> // Header for _pext_u64() intrinsic
#  define pext(b, m) _pext_u64(b, m)
#else
#  define pext(b, m) 0
#endif

#ifdef USE_PEXT
constexpr bool HasPext = true;
#else
constexpr bool HasPext = false;
#endif

#ifdef IS_64BIT
constexpr bool Is64Bit = true;
#else
constexpr bool Is64Bit = false;
#endif