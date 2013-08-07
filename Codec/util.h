#ifndef CODEC_UTIL_H
#define CODEC_UTIL_H

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
#if _MSC_VER < 1600
typedef __int8 int8_t;
typedef unsigned __int8 uint8_t;
typedef __int16 int16_t;
typedef unsigned __int16 uint16_t;
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#else
#include <cstdint>
#endif
#define ZLIB_WINAPI
#endif

namespace Codec
{

template <int N>
struct IntN;

template <>
struct IntN<1>
{
    typedef uint8_t u;
    typedef int8_t s;
};

template <>
struct IntN<2>
{
    typedef uint16_t u;
    typedef int16_t s;
};

template <>
struct IntN<4>
{
    typedef uint32_t u;
    typedef int32_t s;
};

template <>
struct IntN<8>
{
    typedef uint64_t u;
    typedef int64_t s;
};

template <typename From, typename To>
struct CopyConst
{
    typedef To t;
};

template <typename From, typename To>
struct CopyConst<const From, To>
{
    typedef const To t;
};

template <typename T>
struct ArrayDeleter
{
    void operator()(T *p) { delete[] p; }
};

}

#endif //CODEC_UTIL_H