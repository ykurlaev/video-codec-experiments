namespace Codec
{

template <typename Iterator>
inline void Normalize::operator()(Iterator begin, Iterator end)
{
    for(; begin != end; ++begin)
    {
        typename Iterator::value_type t = *begin;
        if(t < 0)
        {
            t = 0;
        }
        if(t > 0xFF)
        {
            t = 0xFF;
        }
        *begin = t;
    }
}

}