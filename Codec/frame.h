#ifndef CODEC_FRAME_H
#define CODEC_FRAME_H

#include <vector>
#include <iterator>
#include "util.h"

namespace Codec
{

template <uint32_t N>
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
        Iterator end() const;
        template <typename F>
        void apply(F &f);
        template <typename F>
        void applyHorizontal(F &f);
        template <typename F>
        void applyVertical(F &f);
        template <typename F>
        void applyScanning(F &f, const coord_t *scan);
    private:
        coord_t m_width;
        coord_t m_height;
        coord_t m_alignedWidth;
        coord_t m_alignedHeight;
        std::vector<data_t> m_data;
        template <uint32_t>
        friend void swap(Frame &first, Frame &second);
};

template <uint32_t N>
void swap(Frame<N> &first, Frame<N> &second);

template <uint32_t N>
template <typename T>
class Frame<N>::BaseIterator
    : public std::iterator<std::forward_iterator_tag, typename Frame<N>::data_t>
{
   private:
        typedef T this_t;
   public:
        static const uint32_t BLOCK_SIZE = N;
        bool operator==(const BaseIterator& other) const;
        bool operator!=(const BaseIterator& other) const;
        typename Frame<N>::template BaseIterator<T>::reference operator*();
        typename Frame<N>::template BaseIterator<T>::this_t operator++(int);
    protected:
        data_t *m_ptr;
        BaseIterator(data_t *ptr);
};

template <uint32_t N>
class Frame<N>::Iterator : public Frame<N>::template BaseIterator<Iterator>
{
    public:
        Iterator();
        Iterator(Frame &frame);
        Iterator &operator++();
    private:
        data_t *m_origin;
        coord_t m_x;
        coord_t m_y;
        coord_t m_width;
        coord_t m_height;
        coord_t m_skip;
};

template <uint32_t N>
class Frame<N>::HorizontalIterator : public Frame<N>::template BaseIterator<HorizontalIterator>
{
    public:
        HorizontalIterator();
        HorizontalIterator(data_t *ptr);
        HorizontalIterator &operator++();
};

template <uint32_t N>
class Frame<N>::VerticalIterator : public Frame<N>::template BaseIterator<VerticalIterator>
{
    public:
        VerticalIterator();
        VerticalIterator(data_t *ptr, coord_t count);
        VerticalIterator &operator++();
    private:
        data_t *m_origin;
        coord_t m_x;
        coord_t m_y;
        coord_t m_block;
        coord_t m_count;
};

template <uint32_t N>
class Frame<N>::ScanningIterator : public Frame<N>::template BaseIterator<ScanningIterator>
{
    public:
        ScanningIterator();
        ScanningIterator(data_t *ptr, coord_t count, const coord_t *scan);
        ScanningIterator &operator++();
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