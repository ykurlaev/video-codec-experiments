#include <cassert>
#include <iterator>

namespace Codec
{

#define CODEC_DCT_FLIP(x, y) \
    { typename std::iterator_traits<Iterator>::value_type t = x; x = x + y; y = t - y; }

//see BinDCT.pdf
#define CODEC_DCT_FORWARD(a, b, c, d, e, f, g, h) \
{ \
    CODEC_DCT_FLIP(a, h); \
    CODEC_DCT_FLIP(b, g); \
    CODEC_DCT_FLIP(c, f); \
    CODEC_DCT_FLIP(d, e); \
    f = f - (7 * g) / 16; \
    g = g + (11 * f) / 16; \
    f = (3 * g) / 8 - f; \
    CODEC_DCT_FLIP(a, d); \
    CODEC_DCT_FLIP(b, c); \
    a = a + b; \
    b = a / 2 - b; \
    c = c - (3 * d) / 8; \
    d = d + (3 * c) / 8; \
    CODEC_DCT_FLIP(e, f); \
    CODEC_DCT_FLIP(h, g); \
    e = e - (3 * h) / 16; \
    h = h + (3 * e) / 16; \
    f = f + (7 * g) / 8; \
    g = g - f / 2; \
    typename std::iterator_traits<Iterator>::value_type t = b; \
    b = h; \
    h = e; \
    e = t; \
    t = c; \
    c = d; \
    d = g; \
    g = t; \
}

#define CODEC_DCT_REVERSE(a, b, c, d, e, f, g, h) \
{ \
    typename std::iterator_traits<Iterator>::value_type t = g; \
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
    CODEC_DCT_FLIP(b, c); \
    CODEC_DCT_FLIP(a, d); \
    g = g + f / 2; \
    f = f - (7 * g) / 8; \
    h = h - (3 * e) / 16; \
    e = e + (3 * h) / 16; \
    CODEC_DCT_FLIP(e, f); \
    CODEC_DCT_FLIP(h, g); \
    f = (3 * g) / 8 - f; \
    g = g - (11 * f) / 16; \
    f = f + (7 * g) / 16; \
    CODEC_DCT_FLIP(d, e); \
    CODEC_DCT_FLIP(c, f); \
    CODEC_DCT_FLIP(b, g); \
    CODEC_DCT_FLIP(a, h); \
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
        CODEC_DCT_FORWARD(*ps[0], *ps[1], *ps[2], *ps[3], *ps[4], *ps[5], *ps[6], *ps[7]);
    }
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
        CODEC_DCT_REVERSE(*ps[0], *ps[1], *ps[2], *ps[3], *ps[4], *ps[5], *ps[6], *ps[7]);
        for(int i = 0; i < 8; i++)
        {
            *ps[i] /= 4;
        }
    }
}

}
