#ifndef CODEC_FRAMETEST_H
#define CODEC_FRAMETEST_H

#include "test.h"
#include "../src/lib/util.h"

class FrameTest : public Test
{
    public:
        virtual void operator()();
    private:
        static const uint8_t region[];
        static const uint8_t horizontal[];
        static const uint8_t vertical[];
        static const uint8_t scanning[];
        static bool testFrameIterators();
        template <typename Iterator>
        static void check(Iterator begin, Iterator end, const char *whyFail);
};

template <typename Iterator>
inline void FrameTest::check(Iterator begin, Iterator end, const char *whyFail)
{
    for(int i = 1; begin != end; ++begin, i++)
    {
        if(*begin != i)
        {
            fail(whyFail);
        }
    }
}

#endif //CODEC_FRAMETEST_H
