#include <cassert>

namespace Codec
{

#define FLIP(x, y) { typename Iterator::value_type t = x; x = x + y; y = t - y; }

//see BinDCT.pdf
#define STEP(A, B, C, D, E, F, G, H) \
{ \
    typename Iterator::value_type t = g; \
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
    FLIP(b, c); \
    FLIP(a, d); \
    g = g + f / 2; \
    f = f - (7 * g) / 8; \
    h = h - (3 * e) / 16; \
    e = e + (3 * h) / 16; \
    FLIP(e, f); \
    FLIP(h, g); \
    f = (3 * g) / 8 - f; \
    g = g - (11 * f) / 16; \
    f = f + (7 * g) / 16; \
    FLIP(d, e); \
    FLIP(c, f); \
    FLIP(b, g); \
    FLIP(a, h); \
}

template <typename Iterator>
inline void UnDCT::operator()(Iterator begin, Iterator end)
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