#include <iostream>
#include "encoder/encode.h"
#include "decoder/decode.h"

#include <time.h>
#include <math.h>
#include "encoder/dct.h"
#include "encoder/quantize.h"
#include "decoder/dequantize.h"
#include "decoder/undct.h"

using namespace Codec;

using std::cerr;

static int usage(const char *name)
{
    cerr << "Usage: " << name << " -e <file.y> <width> <height> <file.compressed>\n";
    cerr << "       " << name << " -d <file.compressed> <file.y>\n";
    return 1;
}

struct It : std::iterator<std::forward_iterator_tag, int>
{
    It(int *c = NULL)
        : x(0), y(0), c(c), o(c)
    {}
    bool operator!=(const It &other)
    {
        return c != other.c;
    }
    int &operator*()
    {
        return *c;
    }
    void operator++()
    {
        y++;
        if(y == 8)
        {
            y = 0;
            x++;
            if(x == 8)
            {
                c = NULL;
            }
            else
            {
                o++;
                c = o;
            }
        }
        else
        {
            c += 8;
        }
    }
    int x, y, *c, *o;
};

void f(int n, int *a)
{
    static int o[3][64], t[3][64];
    for(int y = 0; y < 8; y++)
    {
        for(int x = 0; x < 8; x++)
        {
            if(x < n && y < n)
            {
                for(int i = 0; i < 3; i++)
                {
                    t[i][y * 8 + x] = o[i][y * 8 + x] = a[y * n + x];
                }
            }
            else
            {
                o[0][y * 8 + x] = 0;
                o[1][y * 8 + x] = a[((y < n) ? y : (n - 1)) * n + ((x < n) ? x : (n - 1))];
                o[2][y * 8 + x] = a[(y % n) * n + (x % n)];
                for(int i = 0; i < 3; i++)
                {
                    t[i][y * 8 + x] = o[i][y * 8 + x];
                }
            }
        }
    }
    DCT dct;
    Quantize q(true, 64);
    Dequantize iq(true, 64);
    UnDCT idct;
    for(int i = 0; i < 3; i++)
    {
        cerr << "Original:\n";
        for(int y = 0; y < 8; y++)
        {
            for(int x = 0; x < 8; x++)
            {
                cerr << t[i][y * 8 + x] << " ";
            }
            cerr << "\n";
        }
        dct(t[i], t[i] + 64);
        dct(It(t[i]), It());
        float avg = 0, dev = 0;
        cerr << "Transformed:\n";
        for(int y = 0; y < 8; y++)
        {
            for(int x = 0; x < 8; x++)
            {
                if(x > 0 || y > 0)
                {
                    avg += t[i][y * 8 + x];
                }
            }
        }
        avg /= 63;
        for(int y = 0; y < 8; y++)
        {
            for(int x = 0; x < 8; x++)
            {
                if(x > 0 || y > 0)
                {
                    dev += (t[i][y * 8 + x] - avg) * (t[i][y * 8 + x] - avg);
                }
                cerr << t[i][y * 8 + x] << " ";
            }
            cerr << "\n";
        }
        dev /= 63;
        dev = sqrtf(dev);
        cerr << "Average=" << avg <<", deviation=" << dev << "\n";
        q(t[i], t[i] + 64);
        iq(t[i], t[i] + 64);
        idct(t[i], t[i] + 64);
        idct(It(t[i]), It());
        long long avd = 0;
        cerr << "Processed:\n";
        for(int y = 0; y < 8; y++)
        {
            for(int x = 0; x < 8; x++)
            {
                if(x < n && y < n)
                {
                    avd += abs(t[i][y * 8 + x] - o[i][y * 8 + x]);
                }
                cerr << t[i][y * 8 + x] << " ";
            }
            cerr << "\n";
        }
        avg /= n * n;
        cerr << "Average differnce=" << avd << "\n";
        cerr << "\n";
    }
    cerr << "\n";
    cerr << "\n";
}

void f1(int n, int *a)
{
    static int o[3][8], t[3][8];
    for(int x = 0; x < 8; x++)
    {
        if(x < n)
        {
            for(int i = 0; i < 3; i++)
            {
                t[i][x] = o[i][x] = a[x];
            }
        }
        else
        {
            o[0][x] = 0;
            o[1][x] = a[(x < n) ? x : (n - 1)];
            o[2][x] = a[x % n];
            for(int i = 0; i < 3; i++)
            {
                t[i][x] = o[i][x];
            }
        }
    }
    DCT dct;
    Quantize q(true, 64);
    Dequantize iq(true, 64);
    UnDCT idct;
    for(int i = 0; i < 3; i++)
    {
        cerr << "Original:\n";
        for(int x = 0; x < 8; x++)
        {
            cerr << t[i][x] << " ";
        }
        cerr << "\n";
        dct(t[i], t[i] + 8);
        float avg = 0, dev = 0;
        cerr << "Transformed:\n";
        for(int x = 0; x < 8; x++)
        {
            if(x > 0)
            {
                avg += t[i][x];
            }
        }
        avg /= 7;
        for(int x = 0; x < 8; x++)
        {
            if(x > 0)
            {
                dev += (t[i][x] - avg) * (t[i][x] - avg);
            }
            cerr << t[i][x] << " ";
        }
        cerr << "\n";
        dev /= 7;
        dev = sqrtf(dev);
        cerr << "Average=" << avg <<", deviation=" << dev << "\n";
        q(t[i], t[i] + 8);
        iq(t[i], t[i] + 8);
        idct(t[i], t[i] + 8);
        long long avd = 0;
        cerr << "Processed:\n";
        for(int x = 0; x < 8; x++)
        {
            if(x < n)
            {
                avd += abs(t[i][x] - o[i][x]);
            }
            cerr << t[i][x] << " ";
        }
        cerr << "\n";
        avg /= n;
        cerr << "Average differnce=" << avd << "\n";
        cerr << "\n";
    }
    cerr << "\n";
    cerr << "\n";
}

int main(int argc, char *argv[])
{
    //if(argc > 1 && !strcmp(argv[1], "-e"))
    //{
    //    int ret = encode(argc - 2, argv + 2);
    //    if(ret == 1)
    //    {
    //        return usage(argv[0]);
    //    }
    //    return ret;
    //}
    //if(argc > 1 && !strcmp(argv[1], "-d"))
    //{
    //    int ret = decode(argc - 2, argv + 2);
    //    if(ret == 1)
    //    {
    //        return usage(argv[0]);
    //    }
    //    return ret;
    //}
    //return usage(argc > 0 ? argv[0] : "[program]");
    /*int t1[9] = {   0, 255, 255,
                    0,   0,   0,
                  255,   0,   0 };
    f(3, t1);
    int t2[16] = { 255,   0,   0, 255,
                     0,   0, 255,   0,
                     0,   0,   0,   0,
                   255, 255,   0,   0 };
    f(4, t2);
    int t3[9] = { 255, 170,  85,
                  255, 170,  85,
                  255, 170,  85 };
    f(3, t3);
    int t4[16] = { 255, 191, 127,  63,
                   255, 191, 127,  63,
                   255, 191, 127,  63,
                   255, 191, 127,  63 };
    f(4, t4);*/
    srand(time(NULL));
    int t1[1] = { rand() % 256 };
    int t2[2] = { rand() % 256, rand() % 256 };
    int t3[3] = { rand() % 256, rand() % 256, rand() % 256 };
    int t4[4] = { rand() % 256, rand() % 256, rand() % 256, rand() % 256 };
    int t5[5] = { rand() % 256, rand() % 256, rand() % 256, rand() % 256,
                  rand() % 256 };
    int t6[6] = { rand() % 256, rand() % 256, rand() % 256, rand() % 256,
                  rand() % 256, rand() % 256 };
    int t7[7] = { rand() % 256, rand() % 256, rand() % 256, rand() % 256,
                  rand() % 256, rand() % 256, rand() % 256 };
    int t8[8] = { rand() % 256, rand() % 256, rand() % 256, rand() % 256,
                  rand() % 256, rand() % 256, rand() % 256, rand() % 256 };
    f1(1, t1);
    f1(2, t2);
    f1(3, t3);
    f1(4, t4);
    f1(5, t5);
    f1(6, t6);
    f1(7, t7);
    f1(8, t8);
}