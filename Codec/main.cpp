#include <iostream>
#include "encoder/encode.h"
#include "decoder/decode.h"

#include "frame.h"

using namespace Codec;

using std::cerr;

static int usage(const char *name)
{
    cerr << "Usage: " << name << " -e <file.y> <width> <height> <file.compressed>\n";
    cerr << "       " << name << " -d <file.compressed> <file.y>\n";
    return 1;
}

int main(int argc, char *argv[])
{
    if(argc > 1 && !strcmp(argv[1], "-e"))
    {
        int ret = encode(argc - 2, argv + 2);
        if(ret == 1)
        {
            return usage(argv[0]);
        }
        return ret;
    }
    if(argc > 1 && !strcmp(argv[1], "-d"))
    {
        int ret = decode(argc - 2, argv + 2);
        if(ret == 1)
        {
            return usage(argv[0]);
        }
        return ret;
    }
    return usage(argc > 0 ? argv[0] : "[program]");
}