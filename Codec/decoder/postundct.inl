namespace Codec
{

template <typename Iterator>
inline void PostUnDCT::operator()(Iterator begin, Iterator end)
{
    for(; begin != end; ++begin)
    {
        typename Iterator::value_type t = *begin;
        t /= 16;
        if(t > 0xFF)
        {
            t = 0xFF;
        }
        if(t < 0)
        {
            t = 0;
        }
        *begin = t;
    }
}

}