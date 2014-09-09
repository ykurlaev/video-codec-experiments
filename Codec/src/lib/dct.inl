#include <cassert>
#include <iterator>

namespace Codec
{

#define CODEC_DCT_FLIP(T, x, y) \
    { T t = x; x = x + y; y = t - y; }

//see BinDCT.pdf
#define CODEC_DCT_FORWARD(T, a, b, c, d, e, f, g, h) \
{ \
    CODEC_DCT_FLIP(T, a, h); \
    CODEC_DCT_FLIP(T, b, g); \
    CODEC_DCT_FLIP(T, c, f); \
    CODEC_DCT_FLIP(T, d, e); \
    f = f - (7 * g) / 16; \
    g = g + (11 * f) / 16; \
    f = (3 * g) / 8 - f; \
    CODEC_DCT_FLIP(T, a, d); \
    CODEC_DCT_FLIP(T, b, c); \
    a = a + b; \
    b = a / 2 - b; \
    c = c - (3 * d) / 8; \
    d = d + (3 * c) / 8; \
    CODEC_DCT_FLIP(T, e, f); \
    CODEC_DCT_FLIP(T, h, g); \
    e = e - (3 * h) / 16; \
    h = h + (3 * e) / 16; \
    f = f + (7 * g) / 8; \
    g = g - f / 2; \
    T t = b; \
    b = h; \
    h = e; \
    e = t; \
    t = c; \
    c = d; \
    d = g; \
    g = t; \
}

#define CODEC_DCT_REVERSE(T, a, b, c, d, e, f, g, h) \
{ \
    T t = g; \
    g = d; \
    d = c; \
    c = t; \
    t = e; \
    e = h; \
    h = b; \
    b = t; \
    b = a / 2 - b; \
    a = a - b; \
    d = d - (3 * c) / 8; \
    c = c + (3 * d) / 8; \
    CODEC_DCT_FLIP(T, b, c); \
    CODEC_DCT_FLIP(T, a, d); \
    g = g + f / 2; \
    f = f - (7 * g) / 8; \
    h = h - (3 * e) / 16; \
    e = e + (3 * h) / 16; \
    CODEC_DCT_FLIP(T, e, f); \
    CODEC_DCT_FLIP(T, h, g); \
    f = (3 * g) / 8 - f; \
    g = g - (11 * f) / 16; \
    f = f + (7 * g) / 16; \
    CODEC_DCT_FLIP(T, d, e); \
    CODEC_DCT_FLIP(T, c, f); \
    CODEC_DCT_FLIP(T, b, g); \
    CODEC_DCT_FLIP(T, a, h); \
    a /= 4; \
    b /= 4; \
    c /= 4; \
    d /= 4; \
    e /= 4; \
    f /= 4; \
    g /= 4; \
    h /= 4; \
}

template <typename T>
inline void DCT::applyForward(T &a, T &b, T &c, T &d, T &e, T &f, T &g, T &h)
{
    CODEC_DCT_FORWARD(T, a, b, c, d, e, f, g, h);
}

template <typename Iterator>
inline void DCT::applyForward(Iterator begin, Iterator end)
{
    while(begin != end)
    {
        typename std::iterator_traits<Iterator>::pointer ps[8];
        for(int i = 0; i < 8; i++)
        {
            assert(begin != end);
            ps[i] = &*begin;
            ++begin;
        }
        CODEC_DCT_FORWARD(typename std::iterator_traits<Iterator>::value_type,
                          *ps[0], *ps[1], *ps[2], *ps[3], *ps[4], *ps[5], *ps[6], *ps[7]);
    }
}

template <typename T>
inline void DCT::applyReverse(T &a, T &b, T &c, T &d, T &e, T &f, T &g, T &h)
{
    CODEC_DCT_REVERSE(T, a, b, c, d, e, f, g, h);
}

template <typename Iterator>
inline void DCT::applyReverse(Iterator begin, Iterator end)
{
    while(begin != end)
    {
        typename std::iterator_traits<Iterator>::pointer ps[8];
        for(int i = 0; i < 8; i++)
        {
            assert(begin != end);
            ps[i] = &*begin;
            ++begin;
        }
        CODEC_DCT_REVERSE(typename std::iterator_traits<Iterator>::value_type,
                          *ps[0], *ps[1], *ps[2], *ps[3], *ps[4], *ps[5], *ps[6], *ps[7]);
    }
}

}
