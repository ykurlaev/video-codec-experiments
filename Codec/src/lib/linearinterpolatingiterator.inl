namespace Codec
{

template <typename OtherIter>
LinearInterpolatingIterator<OtherIter>::LinearInterpolatingIterator(OtherIter first, 
                                                                    OtherIter second,
                                                                    int firstX, int secondX)
    : m_first(first), m_second(second), m_firstX(firstX), m_secondX(secondX)
{}

template <typename OtherIter>
bool LinearInterpolatingIterator<OtherIter>::operator==(const LinearInterpolatingIterator& other) const
{
    return (m_first == other.m_first) && (m_second == other.m_second);
}

template <typename OtherIter>
bool LinearInterpolatingIterator<OtherIter>::operator!=(const LinearInterpolatingIterator& other) const
{
    return !(*this == other);
}

template <typename OtherIter>
typename std::iterator_traits<OtherIter>::value_type LinearInterpolatingIterator<OtherIter>::operator*()
{
    typedef typename std::iterator_traits<OtherIter>::value_type T;
    T first = *m_first;
    T second = *m_second;
    return first - m_firstX * ((second - first) / (m_secondX - m_firstX));
}

template <typename OtherIter>
LinearInterpolatingIterator<OtherIter> &LinearInterpolatingIterator<OtherIter>::operator++()
{
    ++m_first;
    ++m_second;
    return *this;
}

template <typename OtherIter>
LinearInterpolatingIterator<OtherIter> LinearInterpolatingIterator<OtherIter>::operator++(int)
{
    LinearInterpolatingIterator<OtherIter> result(*this);
    ++m_first;
    ++m_second;
    return result;
}

template <typename OtherIter>
LinearInterpolatingIterator<OtherIter> makeLinearInterpolatingIterator(OtherIter first, 
                                                                       OtherIter second,
                                                                       int firstX, int secondX)
{
    return LinearInterpolatingIterator<OtherIter>(first, second, firstX, secondX);
}

}
