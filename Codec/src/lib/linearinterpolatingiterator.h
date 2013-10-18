#ifndef CODEC_LINEARINTERPOLATINGITERATOR_H
#define CODEC_LINEARINTERPOLATINGITERATOR_H

#include <iterator>

namespace Codec
{

template <typename OtherIter>
class LinearInterpolatingIterator
    : public std::iterator<std::input_iterator_tag, typename std::iterator_traits<OtherIter>::value_type>
{
    public:
        LinearInterpolatingIterator(OtherIter first, OtherIter second, int firstX, int secondX);
        bool operator==(const LinearInterpolatingIterator& other) const;
        bool operator!=(const LinearInterpolatingIterator& other) const;
        typename std::iterator_traits<OtherIter>::value_type operator*();
        LinearInterpolatingIterator &operator++();
        LinearInterpolatingIterator operator++(int);
    private:
        OtherIter m_first;
        OtherIter m_second;
        int m_firstX;
        int m_secondX;
};

template <typename OtherIter>
LinearInterpolatingIterator<OtherIter> makeLinearInterpolatingIterator(OtherIter first,
                                                                       OtherIter second,
                                                                       int firstX, int secondX);

}

#include "linearinterpolatingiterator.inl"

#endif //CODEC_LINEARINTERPOLATINGITERATOR_H
