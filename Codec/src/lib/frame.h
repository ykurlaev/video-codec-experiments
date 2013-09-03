#ifndef CODEC_FRAME_H
#define CODEC_FRAME_H

#include <vector>
#include <iterator>
#include "util.h"

namespace Codec
{

template <uint32_t MIN_N = 0, uint32_t MAX_N = 0>
class Frame
{
    private:
        template <typename T>
        class BaseIterator;
        class RegionIterator;
        class HorizontalBlockIterator;
        class VerticalBlockIterator;
        class ScanningBlockIterator;
    public:
        static const uint32_t MIN_BLOCK_SIZE = MIN_N;
        static const uint32_t MAX_BLOCK_SIZE = MAX_N;
        typedef uint32_t coord_t;
        typedef int data_t;
        Frame(coord_t width, coord_t height);
        coord_t getWidth() const;
        coord_t getHeight() const;
        coord_t getAlignedWidth() const;
        coord_t getAlignedHeight() const;
        void clear();
        void fromByteArray(const uint8_t *byteArray);
        void toByteArray(uint8_t *byteArray);
        RegionIterator begin();
        RegionIterator end();
        RegionIterator regionBegin(coord_t xstart, coord_t ystart, coord_t width, coord_t height);
        RegionIterator regionEnd();
        HorizontalBlockIterator horizontalBegin(coord_t block = 0);
        HorizontalBlockIterator horizontalEnd();
        VerticalBlockIterator verticalBegin(coord_t block = 0);
        VerticalBlockIterator verticalEnd();
        ScanningBlockIterator scanningBegin(const coord_t *scan, coord_t block = 0);
        ScanningBlockIterator scanningEnd();
    private:
        coord_t m_width;
        coord_t m_height;
        coord_t m_alignedWidth;
        coord_t m_alignedHeight;
        std::vector<data_t> m_data;
        std::vector<data_t *> m_ptrs;
        template <uint32_t MIN_M, uint32_t MAX_M>
        friend void swap(Frame<MIN_M, MAX_M> &first, Frame<MIN_M, MAX_M> &second);
};

template <uint32_t MIN_N, uint32_t MAX_N>
void swap(Frame<MIN_N, MAX_N> &first, Frame<MIN_N, MAX_N> &second);

template <uint32_t MIN_N, uint32_t MAX_N>
template <typename T>
class Frame<MIN_N, MAX_N>::BaseIterator
    : public std::iterator<std::forward_iterator_tag, typename Frame<MIN_N, MAX_N>::data_t>
{
   public:
        bool operator==(const T& other) const;
        bool operator!=(const T& other) const;
        typename Frame<MIN_N, MAX_N>::template BaseIterator<T>::reference operator*();
        T &operator++();
        T operator++(int);
    protected:
        data_t *m_ptr;
        BaseIterator(data_t *ptr);
};

template <uint32_t MIN_N, uint32_t MAX_N>
class Frame<MIN_N, MAX_N>::RegionIterator : public BaseIterator<RegionIterator>
{
    public:
        RegionIterator(data_t *ptr = NULL, data_t **pptr = NULL, coord_t x = 0, coord_t y = 0,
                       coord_t xstart = 0, coord_t xend = 0, coord_t yend = 0, coord_t skip = 0);
        void increment();
    private:
        data_t **m_pptr;
        coord_t m_x;
        coord_t m_y;
        coord_t m_xstart;
        coord_t m_xend;
        coord_t m_yend;
        coord_t m_skip;
};

template <uint32_t MIN_N, uint32_t MAX_N>
class Frame<MIN_N, MAX_N>::HorizontalBlockIterator : public BaseIterator<HorizontalBlockIterator>
{
    public:
        HorizontalBlockIterator(data_t *ptr = NULL);
        void increment();
};

template <uint32_t MIN_N, uint32_t MAX_N>
class Frame<MIN_N, MAX_N>::VerticalBlockIterator : public BaseIterator<VerticalBlockIterator>
{
    public:
        VerticalBlockIterator(data_t *ptr = NULL, coord_t count = 0);
        void increment();
    private:
        data_t *m_origin;
        coord_t m_x;
        coord_t m_y;
        coord_t m_block;
        coord_t m_count;
};

template <uint32_t MIN_N, uint32_t MAX_N>
class Frame<MIN_N, MAX_N>::ScanningBlockIterator : public BaseIterator<ScanningBlockIterator>
{
    public:
        ScanningBlockIterator(data_t *ptr = NULL, coord_t count = 0, const coord_t *scan = NULL);
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
