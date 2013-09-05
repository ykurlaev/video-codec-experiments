namespace Codec
{

template <typename Iterator>
typename std::iterator_traits<Iterator>::value_type FindAverage::operator()(Iterator begin, Iterator end)
{
    typename std::iterator_traits<Iterator>::value_type sum = 0;
    typename std::iterator_traits<Iterator>::difference_type i = 0;
    for(; begin != end; ++begin, i++)
    {
        sum += *begin;
    }
    return sum / i;
}

}
