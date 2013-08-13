#include <cassert>
#include <iterator>

namespace Codec
{

#define CODEC_DCT_FLIP(x, y) \
    { typename std::iterator_traits<Iterator>::value_type t = x; x = x + y; y = t - y; }

//see BinDCT.pdf
#define CODEC_DCT(A, B, C, D, E, F, G, H) \
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

template <typename Iterator>
inline void DCT::operator()(Iterator begin, Iterator end)
{
    while(begin != end)
    {
        typename std::iterator_traits<Iterator>::reference a = *begin;
        ++begin;
        assert(begin != end);
        typename std::iterator_traits<Iterator>::reference b = *begin;
        ++begin;
        assert(begin != end);
        typename std::iterator_traits<Iterator>::reference c = *begin;
        ++begin;
        assert(begin != end);
        typename std::iterator_traits<Iterator>::reference d = *begin;
        ++begin;
        assert(begin != end);
        typename std::iterator_traits<Iterator>::reference e = *begin;
        ++begin;
        assert(begin != end);
        typename std::iterator_traits<Iterator>::reference f = *begin;
        ++begin;
        assert(begin != end);
        typename std::iterator_traits<Iterator>::reference g = *begin;
        ++begin;
        assert(begin != end);
        typename std::iterator_traits<Iterator>::reference h = *begin;
        ++begin;
        assert(begin != end);
        CODEC_DCT(a, b, c, d, e, f, g, h);
    }
}

}