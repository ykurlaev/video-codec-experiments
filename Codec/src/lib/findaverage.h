#ifndef CODEC_FINDAVERAGE_H
#define CODEC_FINDAVERAGE_H

#include "util.h"

namespace Codec
{

class FindAverage
{
    public:
        template <typename Iterator>
        typename std::iterator_traits<Iterator>::value_type operator()(Iterator begin, Iterator end);
};

}

#include "findaverage.inl"

#endif //CODEC_FINDAVERAGE_H
