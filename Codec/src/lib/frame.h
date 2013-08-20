#ifndef CODEC_FRAME_H
#define CODEC_FRAME_H

#include <vector>
#include <iterator>
#include "util.h"

namespace Codec
{

template <uint32_t N = 0>
class Frame
{
    private:
        template <typename T>
        class BaseIterator;
        class Iterator;
        class HorizontalIterator;
        class VerticalIterator;
        class ScanningIterator;
    public:
        static const uint32_t BLOCK_SIZE = N;
        typedef uint32_t coord_t;
        typedef int data_t;
        Frame(coord_t width, coord_t height, coord_t alignment);
        Frame(const Frame &other);
        Frame &operator=(const Frame &other);
        coord_t getWidth() const;
        coord_t getHeight() const;
        coord_t getAlignedWidth() const;
        coord_t getAlignedHeight() const;
        void clear();
        Iterator begin();
        Iterator end();
        HorizontalIterator horizontalBegin();
        HorizontalIterator horizontalEnd();
        VerticalIterator verticalBegin();
        VerticalIterator verticalEnd();
        ScanningIterator scanningBegin(const coord_t *scan);
        ScanningIterator scanningEnd();
    private:
        coord_t m_width;
        coord_t m_height;
        coord_t m_alignedWidth;
        coord_t m_alignedHeight;
        std::vector<data_t> m_data;
        std::vector<data_t *> m_ptrs;
        template <uint32_t M>
        friend void swap(Frame<M> &first, Frame<M> &second);
};

template <uint32_t N>
void swap(Frame<N> &first, Frame<N> &second);

template <uint32_t N>
template <typename T>
class Frame<N>::BaseIterator
    : public std::iterator<std::forward_iterator_tag, typename Frame<N>::data_t>
{
   public:
        bool operator==(const BaseIterator& other) const;
        bool operator!=(const BaseIterator& other) const;
        typename Frame<N>::template BaseIterator<T>::reference operator*();
        T &operator++();
        T operator++(int);
    protected:
        data_t *m_ptr;
        BaseIterator(data_t *ptr);
};

template <uint32_t N>
class Frame<N>::Iterator : public BaseIterator<Iterator>
{
    public:
        Iterator();
        Iterator(Frame &frame);
        void increment();
    private:
        data_t **m_pptr;
        coord_t m_x;
        coord_t m_y;
        coord_t m_width;
        coord_t m_height;
        coord_t m_skip;
};

template <uint32_t N>
class Frame<N>::HorizontalIterator : public BaseIterator<HorizontalIterator>
{
    public:
        HorizontalIterator();
        HorizontalIterator(data_t *ptr);
        void increment();
};

template <uint32_t N>
class Frame<N>::VerticalIterator : public BaseIterator<VerticalIterator>
{
    public:
        VerticalIterator();
        VerticalIterator(data_t *ptr, coord_t count);
        void increment();
    private:
        data_t *m_origin;
        coord_t m_x;
        coord_t m_y;
        coord_t m_block;
        coord_t m_count;
};

template <uint32_t N>
class Frame<N>::ScanningIterator : public BaseIterator<ScanningIterator>
{
    public:
        ScanningIterator();
        ScanningIterator(data_t *ptr, coord_t count, const coord_t *scan);
        void increment();
    private:
        data_t *m_origin;
        coord_t m_block;
        coord_t m_count;
        const coord_t *m_scan;
        const coord_t *m_scanPtr;
};

}

#include "frame.inl"

#endif //CODEC_FRAME_H