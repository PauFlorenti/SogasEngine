#pragma once

#include <assert.h>
#include <memory>
#include <stdint.h>

#ifdef _WIN64
// Window includes
// Reduce the amount of symbols/headers when we include the windows.h
#define WIN32_LEAN_AND_MEAN
// Windows defines a macro min/max as define that interfieres with std::min/max
#define NOMINMAX
// Other macros to remove windows stuff
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP
#define NOMCX
#define NOSERVICE
#define NOHELP
#include <Windows.h>
#endif

// Unsigned
using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

// Signed
using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using f32 = float;
using f64 = double;

// Static assert
#if defined(__clang__) || defined(__gcc__)
#define STATIC_ASSERT __cpp_static_assert
#else
#define STATIC_ASSERT static_assert
#endif

#define ASSERT assert

// Assert size of default types defined above
STATIC_ASSERT(sizeof(u8) == 1, "Expected u8 to be 1 byte.");
STATIC_ASSERT(sizeof(u16) == 2, "Expected u16 to be 2 byte.");
STATIC_ASSERT(sizeof(u32) == 4, "Expected u32 to be 4 byte.");
STATIC_ASSERT(sizeof(u64) == 8, "Expected u64 to be 8 byte.");

STATIC_ASSERT(sizeof(i8) == 1, "Expected i8 to be 1 byte.");
STATIC_ASSERT(sizeof(i16) == 2, "Expected i16 to be 2 byte.");
STATIC_ASSERT(sizeof(i32) == 4, "Expected i32 to be 4 byte.");
STATIC_ASSERT(sizeof(i64) == 8, "Expected i64 to be 8 byte.");

constexpr u32 INVALID_ID = 0xFFFFFFFF;

// Unused macro
#define UNUSED(x) (void)(x);
