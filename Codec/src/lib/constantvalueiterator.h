#ifndef CODEC_CONSTANTVALUEITERATOR_H
#define CODEC_CONSTANTVALUEITERATOR_H

#include <iterator>

namespace Codec
{

template <typename T>
class ConstantValueIterator : public std::iterator<std::input_iterator_tag, T>
{
    public:
        ConstantValueIterator(T value);
        bool operator==(const ConstantValueIterator& other) const;
        bool operator!=(const ConstantValueIterator& other) const;
        T &operator*();
        ConstantValueIterator &operator++();
        ConstantValueIterator operator++(int);
    private:
        T m_value;
};

template <typename T>
ConstantValueIterator<T> makeConstantValueIterator(T value);

}

#include "constantvalueiterator.inl"

#endif //CODEC_CONSTANTVALUEITERATOR_H