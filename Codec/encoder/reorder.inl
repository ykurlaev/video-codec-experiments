#include <exception>

namespace Codec
{

template <typename T>
inline Reorder<T>::Reorder()
{}

template <typename T>
inline void Reorder<T>::operator()(const Region &region, char *bytes)
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
        uT tmp1 = static_cast<uT>(region(x, y));
        uT tmp2 = static_cast<uT>(region(n - x - 1, n - y - 1));
        for(size_t b = 0; b < sizeof(T); b++)
        {
            bytes[i * sizeof(T) + b] = static_cast<char>((tmp1 >> (b * 8)) & 0xFF);
            bytes[(n * n - i - 1) * sizeof(T) + b] = static_cast<char>((tmp2 >> (b * 8)) & 0xFF);
        }
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
inline Reorder<T>::~Reorder()
{}

}