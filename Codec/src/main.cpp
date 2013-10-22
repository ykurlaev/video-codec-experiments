#include <cstring>
#include <iostream>
#include <sstream>
#include "lib/codec.h"

using std::cerr;
using std::istringstream;
using std::string;

static int usage(const char *name)
{
    cerr << "Usage: " << name << " -e <file.y> <width> <height> <file.compressed> [<quality>] [<flat>] [-q]\n";
    cerr << "       " << name << " -d <file.compressed> <file.y>\n";
    return 1;
}

int main(int argc, char *argv[])
{
    if(argc > 1 && !strcmp(argv[1], "-e"))
    {
        argv += 2;
        argc -= 2;
        if(argc < 4)
        {
            return usage(argv[0]);
        }
        FILE *in = fopen(argv[0], "rb");
        if(!in)
        {
            cerr << "Error: Can't open input file " << argv[0] << "\n";
            return usage(argv[0]);
        }
        FILE *out = fopen(argv[3], "wb");
        if(!out)
        {
            cerr << "Error: Can't open output file " << argv[3] << "\n";
            return 2;
        }
        Codec::coord_t width;
        Codec::coord_t height;
        istringstream(argv[1]) >> width;
        istringstream(argv[2]) >> height;
        uint8_t quality = 50;
        if(argc > 4)
        {
            uint32_t uquality = 0;
            istringstream(argv[4]) >> uquality;
            if(uquality != 0)
            {
                quality = static_cast<uint8_t>(uquality);
            }
        }
        bool flat = false;
        if(argc > 5)
        {
            uint32_t uflat = 0;
            istringstream(argv[5]) >> uflat;
            flat = uflat != 0;
        }
        bool silent = false;
        if(argc > 6 && !strncmp(argv[6], "-q", 3))
        {
            silent = true;
        }
        if(!Codec::Codec::encode(in, out, width, height, quality,
                                 flat ? Codec::Format::FLAT : Codec::Format::JPEG, silent))
        {
            return 2;
        }
        else
        {
            return 0;
        }
    }
    if(argc > 1 && !strcmp(argv[1], "-d"))
    {
        argv += 2;
        argc -= 2;
        if(argc < 2)
        {
            return usage(argv[0]);
        }
        FILE *in = fopen(argv[0], "rb");
        if(!in)
        {
            cerr << "Error: Can't open input file " << argv[0] << "\n";
            return 2;
        }
        FILE *out = fopen(argv[1], "wb");
        if(!out)
        {
            cerr << "Error: Can't open output file " << argv[1] << "\n";
            return 2;
        }
        bool silent = false;
        if(argc > 2 && !strncmp(argv[2], "-q", 3))
        {
            silent = true;
        }
        if(!Codec::Codec::decode(in, out, silent))
        {
            return 2;
        }
        else
        {
            return 0;
        }
    }
    return usage(argc > 0 ? argv[0] : "[program]");
}
