#include "codec.h"
#include <cstring>
#ifdef MEASURE_TIME
#include <ctime>
#endif
#include <sstream>
#include <algorithm>
#include <exception>

namespace Codec
{

using std::clock;
using std::cerr;
using std::ostream;
using std::vector;
using std::copy;
using std::exception;

bool Codec::encode(FILE *input, FILE *output,
                   coord_t width, coord_t height,
                   uint8_t quality, Format::QuantizationMode mode,
                   bool silent, ostream *error)
{
    Codec codec(ENCODE, input, output,
                Format::HeaderParams(width, height, mode, quality),
                silent, error);
    return codec.encodeInternal();
}

bool Codec::decode(FILE *input, FILE *output,
                   bool silent, ostream *error)

{
    Format format(input, 1);
    Codec codec(DECODE, input, output, format.readHeader(), silent, error);
    return codec.decodeInternal();
}

Codec::Codec(Direction direction, FILE *input, FILE *output,
             Format::HeaderParams params,
             bool silent, ostream *error)
    : m_direction(direction), m_input(input), m_output(output),
      m_silent(silent), m_error(error), m_params(params),
      m_current(params.m_width, params.m_height), m_previous(params.m_width, params.m_height),
      m_otherPrevious(params.m_width, params.m_height), m_next(params.m_width, params.m_height),
      m_uncompressed(m_current.getWidth() * m_current.getHeight()),
      m_format((direction == ENCODE) ? output : input, m_current.getWidth() * m_current.getHeight() * 4)
{
    for(int i = 0; i < 3; i++)
    {
        m_BFrames.push_back(Frame<SIZE, SIZE>(params.m_width, params.m_height));
    }
    std::vector<Frame<SIZE> *> prevouisFrames;
    prevouisFrames.push_back(&m_previous);
    prevouisFrames.push_back(&m_otherPrevious);
    coord_t macroblockWidth = m_current.getAlignedWidth() / 16;
    coord_t count = (m_current.getAlignedWidth() * m_current.getAlignedHeight()) / (16 * 16);
    m_macroblocks.reserve(count);
    for(coord_t macroblock = 0; macroblock < count; macroblock++)
    {
        coord_t x = macroblock % macroblockWidth;
        coord_t y = macroblock / macroblockWidth;
        Macroblock *neighbors[4] = { NULL, NULL, NULL, NULL };
        if(x > 0)
        {
            neighbors[0] = &m_macroblocks[macroblock - 1];
        }
        if(x > 0 && y > 0)
        {
            neighbors[1] = &m_macroblocks[macroblock - macroblockWidth - 1];
        }
        if(y > 0)
        {
            neighbors[2] = &m_macroblocks[macroblock - macroblockWidth];
        }
        if(x < macroblockWidth && y > 0)
        {
            neighbors[3] = &m_macroblocks[macroblock - macroblockWidth + 1];
        }
        m_macroblocks.push_back(Macroblock(&m_current, prevouisFrames,
                                           macroblock, neighbors, &m_context));
    }
    m_context.m_quantization.setParams(params.m_quantizationMode, params.m_quality);
    if(direction == ENCODE)
    {
        m_format.writeHeader(params);
    }
}

bool Codec::encodeInternal()
{
    try
    {
        if(!m_silent)
        {
            *m_error << "Compressing... ";
        }
#ifdef MEASURE_TIME
        clock_t begin = clock();
#endif
        vector<uint8_t> IPrecompressed(16 * 16 * 4);
        vector<uint8_t> PPrecompressed(16 * 16 * 4);
        vector<uint8_t> P2Precompressed(16 * 16 * 4);
        int maxBCount = m_BFrames.size();
        for(unsigned count = 1; ;)
        {
            swap(m_previous, m_current);
            swap(m_otherPrevious, m_current);
            bool readen = false;
            bool forceI = false;
            int bCount = 0;
            bool end = false;
            while(!readen)
            {
                if(!m_byteArraySerializer.deserializeByteArray(m_input, &m_uncompressed[0], m_uncompressed.size()))
                {
                    end = true;
                    break;
                }
                m_current.clear();
                m_current.fromByteArray(&m_uncompressed[0]);
                forceI = m_findSAD(m_current.horizontalBegin(), m_current.horizontalEnd(),
                                         m_previous.horizontalBegin())
                         > 25 * m_current.getAlignedWidth() * m_current.getAlignedHeight();
                if((bCount < maxBCount) && !forceI)
                {
                    swap(m_current, m_BFrames[bCount]);
                    bCount++;
                }
                else
                {
                    readen = true;
                }
            }
            if(end)
            {
                break;
            }
            int i = 0;
            for(vector<Macroblock>::iterator it = m_macroblocks.begin();
                it != m_macroblocks.end(); ++it, i++)
            {
                uint8_t *buffer = &IPrecompressed[0];
                size_t size = 0;
                if(forceI)
                {
                    it->processForward(Format::I);
                    size = it->precompressTo(&m_format, buffer);
                    it->processReverse();
                    it->chooseMode(Format::I);
                }
                else
                {
                    Format::MacroblockMode macroblockMode = Format::I;
                    it->processForward(Format::I);
                    size = it->precompressTo(&m_format, buffer);
                    it->processReverse();
                    uint8_t *PBuffer = &PPrecompressed[0];
                    it->processForward(Format::P);
                    size_t PSize = it->precompressTo(&m_format, PBuffer);
                    it->processReverse();
                    if(PSize < size)
                    {
                        size = PSize;
                        buffer = PBuffer;
                        macroblockMode = Format::P;
                    }
                    uint8_t *P2Buffer = &P2Precompressed[0];
                    it->processForward(Format::P2);
                    size_t P2Size = it->precompressTo(&m_format, P2Buffer);
                    it->processReverse();
                    if(P2Size < size)
                    {
                        size = P2Size;
                        buffer = P2Buffer;
                        macroblockMode = Format::P2;
                    }
                    it->chooseMode(macroblockMode);
                }
                m_format.writeMacroblock(buffer, size);
            }
            m_format.writeFrame();
            if(!m_silent)
            {
                *m_error << count++ << " ";
            }
            if(bCount != 0)
            {
                swap(m_current, m_otherPrevious);
                swap(m_previous, m_otherPrevious);
            }
            for(int bFrame = 0; bFrame < bCount; bFrame++)
            {
                swap(m_current, m_BFrames[bFrame]);
                int i = 0;
                for(vector<Macroblock>::iterator it = m_macroblocks.begin();
                    it != m_macroblocks.end(); ++it, i++)
                {
                    uint8_t *buffer = &IPrecompressed[0];
                    size_t size = 0;
                    it->processForward(Format::B);
                    size = it->precompressTo(&m_format, buffer);
                    it->processReverse();
                    it->chooseMode(Format::B);
                    m_format.writeMacroblock(buffer, size);
                }
                m_format.writeFrame();
                if(!m_silent)
                {
                    *m_error << count++ << " ";
                }
            }
            if(bCount != 0)
            {
                swap(m_previous, m_otherPrevious);
                swap(m_current, m_otherPrevious);
            }
        }
        if(!m_silent)
        {
            *m_error << "Ok\n";
        }
#ifdef MEASURE_TIME
        clock_t end = clock();
        *m_error << static_cast<double>(end - begin) / CLOCKS_PER_SEC << "\n";
#endif
    }
    catch(const exception &e)
    {
        *m_error << "\nError: " << e.what() << "\n";
        return false;
    }
    return true;
}

bool Codec::decodeInternal()
{
    try
    {
        m_context.m_quantization.setParams(m_params.m_quantizationMode,
                                           m_params.m_quality);
        if(!m_silent)
        {
            *m_error << m_params.m_width << "x" << m_params.m_height << "@"; 
        }
        *m_error << static_cast<int>(m_params.m_quality)
                 << (m_params.m_quantizationMode == Format::FLAT ? " flat\n" : " non-flat\n");
        if(!m_silent)
        {
            *m_error << "Decompressing... ";   
        }
#ifdef MEASURE_TIME
        clock_t begin = clock();
#endif
        for(unsigned count = 1; ; count++)
        {
            m_current.clear();
            if(!m_format.readFrame())
            {
                break;
            }
            if(!m_silent)
            {
                *m_error << count << " ";
            }
            bool bFrame = m_format.getFrameMode() == Format::B;
            int i = 0;
            for(vector<Macroblock>::iterator it = m_macroblocks.begin();
                it != m_macroblocks.end(); ++it, i++)
            {
                it->readFrom(&m_format);
                it->processReverse();
            }
            if(!bFrame)
            {
                swap(m_current, m_previous);
            }
            if(count != 1)
            {
                m_current.toByteArray(&m_uncompressed[0]);
                m_byteArraySerializer.serializeByteArray(&m_uncompressed[0], m_uncompressed.size(), m_output);
            }
            if(!bFrame)
            {
                swap(m_otherPrevious, m_current);
            }
        }
        swap(m_current, m_previous);
        m_current.toByteArray(&m_uncompressed[0]);
        m_byteArraySerializer.serializeByteArray(&m_uncompressed[0], m_uncompressed.size(), m_output);
        if(!m_silent)
        {
            *m_error << "Ok\n";
        }
#ifdef MEASURE_TIME
        clock_t end = clock();
        *m_error << static_cast<double>(end - begin) / CLOCKS_PER_SEC << "\n";
#endif
    }
    catch(const exception &e)
    {
        *m_error << "\nError: " << e.what() << "\n";
        return false;
    }
    return true;
}

}
