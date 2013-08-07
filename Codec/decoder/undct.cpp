#include "undct.h"
#include <exception>

namespace Codec
{

using std::exception;

UnDCT::UnDCT()
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
    Region::data_t t = g; \
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

void UnDCT::operator()(Region &region)
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
    for(Region::iterator it = region.begin(); it != region.end(); ++it)
    {
        Region::data_t t = *it;
        t /= 16;
        if(t < 0)
        {
            t = 0;
        }
        if(t > 0xFF)
        {
            t = 0xFF;
        }
        *it = t;
    }
}

UnDCT::~UnDCT()
{}

}