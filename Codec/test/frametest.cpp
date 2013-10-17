#include "frametest.h"
#include "../src/lib/frame.h"
#include "../src/lib/zigzagscan.h"

using namespace Codec;

void FrameTest::operator()()
{
    TEST_RUN(testFrameIterators);
}

const uint8_t FrameTest::region[] = {  1,  2,  3,  4,
                                       5,  6,  7,  8,
                                       9, 10, 11, 12,
                                      13, 14, 15, 16 };
const uint8_t FrameTest::horizontal[] = {  1,  2,  5,  6,
                                           3,  4,  7,  8,
                                           9, 10, 13, 14,
                                          11, 12, 15, 16 };
const uint8_t FrameTest::vertical[] = {  1,  3,  5,  7,
                                         2,  4,  6,  8,
                                         9, 11, 13, 15,
                                        10, 12, 14, 16 };
const uint8_t FrameTest::scanning[] = {  1,  3,  5,  7,
                                         2,  4,  6,  8,
                                         9, 11, 13, 15,
                                        10, 12, 14, 16 };

bool FrameTest::testFrameIterators()
{
    Frame<2, 4> frame(4, 4);
    frame.fromByteArray(region);
    check(frame.begin(), frame.end(), "Frame::RegionIterator fail");
    frame.fromByteArray(horizontal);
    check(frame.horizontalBegin(), frame.horizontalEnd(), "Frame::HorizontalIterator fail");
    frame.fromByteArray(vertical);
    check(frame.verticalBegin(), frame.verticalEnd(), "Frame::VerticalIterator fail");
    frame.fromByteArray(scanning);
    const coord_t *zigZagScan = ZigZagScan<2>::getScan();
    check(frame.scanningBegin(zigZagScan), frame.scanningEnd(), "Frame::ScanningIterator fail");
    return true;
}
