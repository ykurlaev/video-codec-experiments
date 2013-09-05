namespace Codec
{

template <typename T>
inline ConstantValueIterator<T>::ConstantValueIterator(T value)
    : m_value(value)
{}

template <typename T>
inline bool ConstantValueIterator<T>::operator==(const ConstantValueIterator& other) const
{
    return m_value == other.m_value;
}

template <typename T>
inline bool ConstantValueIterator<T>::operator!=(const ConstantValueIterator& other) const
{
    return !(*this == other);
}

template <typename T>
inline T &ConstantValueIterator<T>::operator*()
{
    return m_value;
}

template <typename T>
inline ConstantValueIterator<T> &ConstantValueIterator<T>::operator++()
{
    return *this;
}

template <typename T>
inline ConstantValueIterator<T> ConstantValueIterator<T>::operator++(int)
{
    return *this;
}

template <typename T>
inline ConstantValueIterator<T> makeConstantValueIterator(T value)
{
    return ConstantValueIterator<T>(value);
}

}
