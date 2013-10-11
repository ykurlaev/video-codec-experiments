#ifndef CODEC_UTIL_H
#define CODEC_UTIL_H

#include <cstddef>
#if defined(_MSC_VER) && _MSC_VER < 1600
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

namespace Codec
{

typedef uint32_t coord_t;
typedef int data_t;

template <size_t N>
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

template <typename T>
struct MakeSigned
{
    typedef typename IntN<sizeof(T)>::s t;
};

template <typename T>
struct MakeUnsigned
{
    typedef typename IntN<sizeof(T)>::u t;
};

}

#endif //CODEC_UTIL_H
