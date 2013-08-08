#include <cassert>

namespace Codec
{

#define FLIP(x, y) { typename Iterator::value_type t = x; x = x + y; y = t - y; }

//see BinDCT.pdf
#define STEP(A, B, C, D, E, F, G, H) \
{ \
    FLIP(a, h); \
    FLIP(b, g); \
    FLIP(c, f); \
    FLIP(d, e); \
    f = f - (7 * g) / 16; \
    g = g + (11 * f) / 16; \
    f = (3 * g) / 8 - f; \
    FLIP(a, d); \
    FLIP(b, c); \
    a = a + b; \
    b = a / 2 - b; \
    c = c - (3 * d) / 8; \
    d = d + (3 * c) / 8; \
    FLIP(e, f); \
    FLIP(h, g); \
    e = e - (3 * h) / 16; \
    h = h + (3 * e) / 16; \
    f = f + (7 * g) / 8; \
    g = g - f / 2; \
    typename Iterator::value_type t = b; \
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
        typename Iterator::reference a = *begin;
        ++begin;
        assert(begin != end);
        typename Iterator::reference b = *begin;
        ++begin;
        assert(begin != end);
        typename Iterator::reference c = *begin;
        ++begin;
        assert(begin != end);
        typename Iterator::reference d = *begin;
        ++begin;
        assert(begin != end);
        typename Iterator::reference e = *begin;
        ++begin;
        assert(begin != end);
        typename Iterator::reference f = *begin;
        ++begin;
        assert(begin != end);
        typename Iterator::reference g = *begin;
        ++begin;
        assert(begin != end);
        typename Iterator::reference h = *begin;
        ++begin;
        assert(begin != end);
        STEP(a, b, c, d, e, f, g, h);
    }
}

}