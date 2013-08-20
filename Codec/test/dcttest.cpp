#include "dcttest.h"
#include "../src/lib/dct.h"

using namespace Codec;

void DCTTest::operator()()
{
    TEST_RUN(testDct);
}

bool DCTTest::testDct()
{
    DCT dct;
    int a[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    int b[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    dct.applyForward(a, a + 8);
    dct.applyReverse(a, a + 8);
    for(int i = 0; i < 8; i++)
    {
        if(a[i] != b[i])
        {
            fail("array after forward and reverse DCT must be equal to original");
        }
    }
    return true;
}
