#include <algorithm>

namespace Codec
{

template <typename Iterator>
inline DelayedIterator<Iterator>::DelayedIterator(const Iterator &it, unsigned delay)
    : m_it(it), m_delay(delay), m_tmp()
{}

template <typename Iterator>
inline DelayedIterator<Iterator>::DelayedIterator(const DelayedIterator &other)
    : m_it(other.m_it), m_delay(other.m_delay), m_tmp(other.m_tmp)
{}

template <typename Iterator>
inline DelayedIterator<Iterator> &DelayedIterator<Iterator>::operator=(const DelayedIterator &other)
{
    DelayedIterator tmp(other);
    swap(*this, other);
    return *this;
}

template <typename Iterator>
inline bool DelayedIterator<Iterator>::operator==(const DelayedIterator &other)
{
    return m_it == other.m_it;
}

template <typename Iterator>
inline bool DelayedIterator<Iterator>::operator!=(const DelayedIterator &other)
{
    return !(*this == other);
}

template <typename Iterator>
inline typename DelayedIterator<Iterator>::reference DelayedIterator<Iterator>::operator*()
{
    if(m_delay > 0)
    {
        return m_tmp;
    }
    else
    {
        return *m_it;
    }
}

template <typename Iterator>
inline DelayedIterator<Iterator> &DelayedIterator<Iterator>::operator++()
{
    if(m_delay > 0)
    {
        m_delay--;
    }
    else
    {
        ++m_it;
    }
    return *this;
}

template <typename Iterator>
inline DelayedIterator<Iterator> DelayedIterator<Iterator>::operator++(int)
{
    DelayedIterator result(*this);
    ++(*this);
    return result;
}

template <typename Iterator>
inline void swap(DelayedIterator<Iterator> &first, DelayedIterator<Iterator> &second)
{
    using std::swap;
    swap(first.m_it, second.m_it);
    swap(first.m_delay, second.m_delay);
}

template <typename Iterator>
inline DelayedIterator<Iterator> delayIterator(const Iterator &it, unsigned delay)
{
    return DelayedIterator<Iterator>(it, delay);
}

}