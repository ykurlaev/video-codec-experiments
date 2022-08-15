#include "codec.h"
#include <cstring>
#ifdef MEASURE_TIME
#include <ctime>
#endif
#include <sstream>
#include <iomanip>
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
      m_otherPrevious(params.m_width, params.m_height),
      m_uncompressed(m_current.getWidth() * m_current.getHeight()),
      m_IPrecompressed(SIZE * SIZE * 4), m_PPrecompressed(SIZE * SIZE * 4), m_P2Precompressed(SIZE * SIZE * 4),
      m_format((direction == ENCODE) ? output : input, m_current.getWidth() * m_current.getHeight() * 4),
      m_desiredBitrate(0), m_oldsad(0), m_oldsaddiff(0)
{
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
    if(params.m_quantizationMode == Format::CBR)
    {
        m_desiredBitrate = params.m_quality * 64;
        params.m_quantizationMode = Format::JPEG;
        params.m_quality = 50;
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
        int noForceICount = 0;
        for(unsigned count = 1; ;)
        {
            swap(m_previous, m_current);
            swap(m_otherPrevious, m_current);
            bool forceI = false;
            bool end = false;
            if(!m_byteArraySerializer.deserializeByteArray(m_input, &m_uncompressed[0], m_uncompressed.size()))
            {
                end = true;
                break;
            }
            m_current.clear();
            m_current.fromByteArray(&m_uncompressed[0]);
            uint32_t sad = m_findSAD(m_current.horizontalBegin(), m_current.horizontalEnd(),
                                     m_previous.horizontalBegin());
            forceI = (sad > 25 * m_current.getAlignedWidth() * m_current.getAlignedHeight())
                     || (noForceICount++ == 100);
            int32_t saddiff = std::abs(static_cast<int32_t>(sad) - static_cast<int32_t>(m_oldsad));
            std::cerr << "\n[" << std::setw(8) << sad << "/" << std::setw(8)
                      << saddiff << "/" << std::setw(8) << std::abs(saddiff - m_oldsaddiff) << "] ";
            m_oldsad = sad;
            m_oldsaddiff = saddiff;
            if(forceI)
            {
                noForceICount = 0;
            }
            if(end)
            {
                break;
            }
            if(forceI)
            {
                process(Format::I);
            }
            else
            {
                process(Format::P);
            }
            if(!m_silent)
            {
                *m_error << count++ << " ";
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

void Codec::process(Format::MacroblockMode mode)
{
    m_format.setFrameMode(Format::P);
    typedef size_t (Codec::*func)();
    func funcs[Format::P2 + 1] = { &Codec::processI, &Codec::processP, NULL };
    size_t size = (this->*funcs[mode])();
    m_format.writeFrame();
    if(m_desiredBitrate != 0)
    {
        recalculateQuality(size);
    }
}

size_t Codec::processI()
{
    size_t result = 0;
    for(vector<Macroblock>::iterator it = m_macroblocks.begin();
        it != m_macroblocks.end(); ++it)
    {
        uint8_t *buffer = &m_IPrecompressed[0];
        it->processForward(Format::I);
        size_t size = it->precompressTo(&m_format, buffer);
        it->processReverse();
        it->chooseMode(Format::I);
        m_format.writeMacroblock(buffer, size);
        result += size;
    }
    return result;
}

size_t Codec::processP()
{
    size_t result = 0;
    int16_t x = 0, y = 0;
    size_t c = 0;
    for(vector<Macroblock>::iterator it = m_macroblocks.begin();
        it != m_macroblocks.end(); ++it, c++)
    {
        uint8_t *buffer = &m_IPrecompressed[0];
        size_t size = 0;
        Format::MacroblockMode macroblockMode = Format::I;
        it->processForward(Format::I);
        size = it->precompressTo(&m_format, buffer);
        it->processReverse();
        uint8_t *PBuffer = &m_PPrecompressed[0];
        it->processForward(Format::P);
        size_t PSize = it->precompressTo(&m_format, PBuffer);
        it->processReverse();
        if(PSize < size)
        {
            size = PSize;
            buffer = PBuffer;
            macroblockMode = Format::P;
        }
        uint8_t *P2Buffer = &m_P2Precompressed[0];
        it->processForward(Format::P2);
        x = it->getParams().m_xMotion;
        y = it->getParams().m_yMotion;
        std::cerr << "\n(" << std::setw(5) << x << ";" << std::setw(8) << y << ") ";
        size_t P2Size = it->precompressTo(&m_format, P2Buffer);
        it->processReverse();
        if(P2Size < size)
        {
            size = P2Size;
            buffer = P2Buffer;
            macroblockMode = Format::P2;
        }
        it->chooseMode(macroblockMode);
        m_format.writeMacroblock(buffer, size);
        result += size;
    }
    //x = static_cast<int16_t>(x / static_cast<float>(c));
    //y = static_cast<int16_t>(y / static_cast<float>(c));
    //std::cerr << "\n(" << std::setw(5) << x << ";" << std::setw(8) << y << ") ";
    return result;
}

bool Codec::decodeInternal()
{
    try
    {
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
            Format::MacroblockMode frameMode;
            if(!m_format.readFrame(&frameMode))
            {
                break;
            }
            if(!m_silent)
            {
                *m_error << count << " ";
            }
            int i = 0;
            for(vector<Macroblock>::iterator it = m_macroblocks.begin();
                it != m_macroblocks.end(); ++it, i++)
            {
                it->readFrom(&m_format);
                it->processReverse();
            }
            m_current.toByteArray(&m_uncompressed[0]);
            m_byteArraySerializer.serializeByteArray(&m_uncompressed[0], m_uncompressed.size(), m_output);
            swap(m_otherPrevious, m_previous);
            swap(m_current, m_previous);
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

void Codec::recalculateQuality(size_t currentBitrate)
{
    m_params.m_quality = static_cast<uint8_t>(m_params.m_quality * static_cast<float>(currentBitrate) / m_desiredBitrate) & 0xFE;
    m_context.m_quantization.setParams(m_params.m_quantizationMode, m_params.m_quality);
}

}
