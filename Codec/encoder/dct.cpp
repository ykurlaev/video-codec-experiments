#include "dct.h"
#include <exception>

namespace Codec
{

using std::exception;

DCT::DCT(void)
{}

#define FLIP(x, y) { Region::data_t t = x; x = x + y; y = t - y; }

//see BinDCT.pdf
#define STEP(A, B, C, D, E, F, G, H) \
{ \
    Region::data_t &a = A; \
    Region::data_t &b = B; \
    Region::data_t &c = C; \
    Region::data_t &d = D; \
    Region::data_t &e = E; \
    Region::data_t &f = F; \
    Region::data_t &g = G; \
    Region::data_t &h = H; \
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
    Region::data_t t = b; \
    b = h; \
    h = e; \
    e = t; \
    t = c; \
    c = d; \
    d = g; \
    g = t; \
}

void DCT::operator()(Region &region)
{
    if(region.getWidth() != 8 || region.getHeight() != 8)
    {
        throw exception("Region must be 8x8");
    }
    for(Region::coord_t y = 0; y < 8; y++)
    {
        STEP(region(0, y), region(1, y), region(2, y), region(3, y),
             region(4, y), region(5, y), region(6, y), region(7, y));
    }
    for(Region::coord_t x = 0; x < 8; x++)
    {
        STEP(region(x, 0), region(x, 1), region(x, 2), region(x, 3),
             region(x, 4), region(x, 5), region(x, 6), region(x, 7));
    }
}

DCT::~DCT(void)
{}

}