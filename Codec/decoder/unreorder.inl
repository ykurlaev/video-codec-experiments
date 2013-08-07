#include <exception>

namespace Codec
{

template <typename T>
inline Unreorder<T>::Unreorder()
{}

template <typename T>
inline void Unreorder<T>::operator()(const char *bytes, Region &region)
{
    if(region.getWidth() != region.getHeight())
    {
        throw exception("Region must be square");
    }
    Region::coord_t n = region.getWidth();
    Region::scoord_t x = 0, y = 0;
    int d = -1;
    for(size_t i = 0; i < (n * n + 1) / 2; i++)
    {
        typedef IntN<sizeof(T)>::u uT;
        uT tmp1 = 0;
        uT tmp2 = 0;
        for(size_t b = 0; b < sizeof(T); b++)
        {
            tmp1 |= static_cast<uint8_t>(bytes[i * sizeof(T) + b]) << (b * 8);
            tmp2 |= static_cast<uint8_t>(bytes[(n * n - i - 1) * sizeof(T) + b]) << (b * 8);
        }
        region(x, y) = static_cast<Region::data_t>(static_cast<T>(tmp1));
        region(n - x - 1,  n - y - 1) = static_cast<Region::data_t>(static_cast<T>(tmp2));
        x += d;
        y -= d;
        if(x < 0)
        {
            x++;
            d = -d;
        }
        else if(y < 0)
        {
            y++;
            d = -d;
        }
    }
}

template <typename T>
inline Unreorder<T>::~Unreorder()
{}

}