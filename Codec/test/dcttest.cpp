#include "dcttest.h"
#include "../src/lib/dct.h"
#include <cstdlib>
#include <algorithm>

using namespace Codec;

using std::abs;
using std::rand;
using std::srand;
using std::generate;
using std::copy;

void DCTTest::operator()()
{
    TEST_RUN(testDctRandom);
    TEST_RUN(testDctBasisForward);
    TEST_RUN(testDctBasisReverse);
}

bool DCTTest::testDctRandom()
{
    DCT dct;
    srand(17);
    for(int i = 0; i < 10; i++)
    {
        int a[8];
        generate(a, a + 8, rand);
        int b[8];
        copy(a, a + 8, b);
        dct.applyForward(a, a + 8);
        dct.applyReverse(a, a + 8);
        for(int i = 0; i < 8; i++)
        {
            if(abs(a[i] - b[i]) > 1)
            {
                fail("array after forward and reverse DCT must be (almost) equal to original");
            }
        }
    }
    return true;
}

bool DCTTest::testDctBasisForward()
{
    DCT dct;
    int basis[2][8] = { { 256, 256, 256, 256, 256, 256, 256, 256 },
                        { 256,   0,   0, 256, 256,   0,   0, 256 } };
    int rbasis[2][8] = { { 2048, 0, 0, 0,   0, 0, 0, 0 },
                         { 1024, 0, 0, 0, 512, 0, 0, 0 } };
    for(int i = 0; i < 2; i++)
    {
        dct.applyForward(basis[i], basis[i] + 8);
        for(int j = 0; j < 8; j++)
        {
            if(basis[i][j] != rbasis[i][j])
            {
                fail("basis function image doesn't match");
            }
        }
    }
    return true;
}

bool DCTTest::testDctBasisReverse()
{
    DCT dct;
    int basis[2][8] = { { 256, 256, 256, 256, 256, 256, 256, 256 },
                        { 256,   0,   0, 256, 256,   0,   0, 256 } };
    int rbasis[2][8] = { { 2048, 0, 0, 0,   0, 0, 0, 0 },
                         { 1024, 0, 0, 0, 512, 0, 0, 0 } };
    for(int i = 0; i < 2; i++)
    {
        dct.applyReverse(rbasis[i], rbasis[i] + 8);
        for(int j = 0; j < 8; j++)
        {
            if(basis[i][j] != rbasis[i][j])
            {
                fail("inverse basis function inverse image doesn't match");
            }
        }
    }
    return true;
}
