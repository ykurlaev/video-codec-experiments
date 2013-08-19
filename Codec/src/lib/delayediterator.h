#ifndef CODEC_DELAYEDITERATOR_H
#define CODEC_DELAYEDITERATOR_H

#include <iterator>

namespace Codec
{

template <typename Iterator>
class DelayedIterator : public std::iterator<std::forward_iterator_tag, typename Iterator::value_type>
{
    public:
        DelayedIterator(const Iterator &it, unsigned delay);
        DelayedIterator(const DelayedIterator &other);
        DelayedIterator &operator=(const DelayedIterator &other);
        bool operator==(const DelayedIterator &other);
        bool operator!=(const DelayedIterator &other);
        typename DelayedIterator::reference operator*();
        DelayedIterator &operator++();
        DelayedIterator operator++(int);
    private:
        Iterator m_it;
        unsigned m_delay;
        typename DelayedIterator::value_type m_tmp;
        template <typename T>
        friend void swap(DelayedIterator<T> &first, DelayedIterator<T> &second);
};

template <typename Iterator>
void swap(DelayedIterator<Iterator> &first, DelayedIterator<Iterator> &second);

template <typename Iterator>
DelayedIterator<Iterator> delayIterator(const Iterator &it, unsigned delay);

}

#include "delayediterator.inl"

#endif //CODEC_DELAYEDITERATOR_H