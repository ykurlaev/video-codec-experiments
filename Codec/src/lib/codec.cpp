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

Codec Codec::initEncode(FILE *input, FILE *output,
                        Frame<>::coord_t width, Frame<>::coord_t height,
                        uint8_t quality, bool flat,
                        bool silent, ostream *error)
{
    ByteArraySerializer byteArraySerializer;
    byteArraySerializer.serializeUint32(width, output);
    byteArraySerializer.serializeUint32(height, output);
    byteArraySerializer.serializeUint32(quality, output);
    byteArraySerializer.serializeUint32(flat ? 1 : 0, output);
    return Codec(ENCODE, input, output, width, height, quality, flat, silent, error);
}

Codec Codec::initDecode(FILE *input, FILE *output,
                        bool silent, ostream *error)

{
    ByteArraySerializer byteArraySerializer;
    uint32_t uwidth;
    uint32_t uheight;
    uint32_t uquality;
    uint32_t uflat;
    byteArraySerializer.deserializeUint32(input, uwidth);
    byteArraySerializer.deserializeUint32(input, uheight);
    byteArraySerializer.deserializeUint32(input, uquality);
    byteArraySerializer.deserializeUint32(input, uflat);
    Frame<>::coord_t width = static_cast<Frame<8>::coord_t>(uwidth);
    Frame<>::coord_t height = static_cast<Frame<8>::coord_t>(uheight);
    uint8_t quality = uquality & 0xFF;
    bool flat = uflat != 0;
    return Codec(DECODE, input, output, width, height, quality, flat, silent, error);
}

bool Codec::operator()()
{
    if(m_direction == ENCODE)
    {
        return encode();
    }
    else
    {
        return decode();
    }
}

Codec::Codec(Direction direction, FILE *input, FILE *output,
             Frame<>::coord_t width, Frame<>::coord_t height,
             uint8_t quality, bool flat, bool silent, ostream *error)
    : m_direction(direction), m_input(input), m_output(output),
      m_width(width), m_height(height),
      m_quality(quality), m_flat(flat), m_silent(silent), m_error(error),
      m_zigZagScan(ZigZagScan<8, 16>::getScan()), m_quantization(flat, quality),
      m_current(width, height), m_previous(m_current.getAlignedWidth(), m_current.getAlignedHeight()),
      m_uncompressed(m_current.getWidth() * m_current.getHeight()),
      m_precompressed(m_current.getAlignedWidth() * m_current.getAlignedHeight() * Precompressor::MAX_BYTES),
      m_compressed(m_precompressed.size()),
      m_macroblockIsInter((m_current.getAlignedWidth() * m_current.getAlignedHeight()) / (16 * 16) / 8),
      m_motionVectorsX((m_current.getAlignedWidth() * m_current.getAlignedHeight()) / (16 * 16)),
      m_motionVectorsY((m_current.getAlignedWidth() * m_current.getAlignedHeight()) / (16 * 16)),
      m_precompressedMeta((m_macroblockIsInter.size() + m_motionVectorsX.size()
                           + m_motionVectorsY.size()) * Precompressor::MAX_BYTES),
      m_compressedMeta(m_precompressedMeta.size()),
      m_macroblockWidth(m_current.getAlignedWidth() / 16)
{ }

bool Codec::encode()
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
        for(unsigned count = 1; ; count++)
        {
            m_previous.fillFrom(m_current.regionBegin(0, 0, m_current.getAlignedWidth(), m_current.getAlignedHeight()),
                                    m_current.regionEnd());
            m_current.clear();
            if(m_byteArraySerializer.deserializeByteArray(m_input, &m_uncompressed[0], m_uncompressed.size(), false)
               != m_uncompressed.size())
            {
                break;
            }
            if(!m_silent)
            {
                *m_error << count << " ";
            }
            m_current.fromByteArray(&m_uncompressed[0]);
            m_precompressor.setByteArray(&m_precompressed[0]);
            m_prevMacroblockAverage = 128;
            bool forceI = false;
            //if(m_findSAD(m_current.horizontalBegin(), m_current.horizontalEnd(), m_previous.horizontalBegin())
            //   > 12 * m_current.getAlignedWidth() * m_current.getAlignedHeight())
            //{
            //    forceI = true;
            //}
            for(Frame<>::coord_t macroblock = 0;
                macroblock < (m_current.getAlignedWidth() * m_current.getAlignedHeight()) / (16 * 16);
                macroblock++)
            {
                processMacroblockForward(macroblock, forceI);
                m_precompressor.applyForward(m_current.scanningBegin(m_zigZagScan, macroblock),
                                             m_current.scanningBegin(m_zigZagScan, macroblock + 1));
                processMacroblockReverse(macroblock);
            }
            m_normalize(m_current.horizontalBegin(), m_current.horizontalEnd());
            uint32_t precompressedSize = m_precompressor.getBytesProcessed();
            m_precompressor.setByteArray(&m_precompressedMeta[0]);
            m_precompressor.applyForward(&m_macroblockIsInter[0], &m_macroblockIsInter[0] + m_macroblockIsInter.size());
            m_precompressor.applyForward(&m_motionVectorsX[0], &m_motionVectorsX[0] + m_motionVectorsX.size());
            m_precompressor.applyForward(&m_motionVectorsY[0], &m_motionVectorsY[0] + m_motionVectorsY.size());
            uint32_t compressedMetaSize = m_zlibCompress(&m_precompressedMeta[0], &m_compressedMeta[0],
                                                         m_precompressor.getBytesProcessed(), m_compressedMeta.size());
            uint32_t compressedSize = m_zlibCompress(&m_precompressed[0], &m_compressed[0],
                                                     precompressedSize, m_compressed.size());
            m_byteArraySerializer.serializeByteArray(&m_compressedMeta[0], compressedMetaSize, m_output);
            m_byteArraySerializer.serializeByteArray(&m_compressed[0], compressedSize, m_output);
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

bool Codec::decode()
{
    try
    {
        if(!m_silent)
        {
            *m_error << m_width << "x" << m_height << "@"; 
        }
        *m_error << static_cast<int>(m_quality) << (m_flat ? " flat\n" : " non-flat\n");
        if(!m_silent)
        {
            *m_error << "Decompressing... ";   
        }
#ifdef MEASURE_TIME
        clock_t begin = clock();
#endif
        for(unsigned count = 1; ; count++)
        {
            uint32_t compressedMetaSize =
                m_byteArraySerializer.deserializeByteArray(m_input, &m_compressedMeta[0], m_compressedMeta.size());
            if(compressedMetaSize == 0)
            {
                break;
            }
            uint32_t compressedSize =
                m_byteArraySerializer.deserializeByteArray(m_input, &m_compressed[0], m_compressed.size());
            if(compressedSize == 0)
            {
                break;
            }
            if(!m_silent)
            {
                *m_error << count << " ";
            }
            m_zlibDecompress(&m_compressedMeta[0], &m_precompressedMeta[0], compressedMetaSize, m_precompressedMeta.size());
            m_zlibDecompress(&m_compressed[0], &m_precompressed[0], compressedSize, m_precompressed.size());
            m_precompressor.setByteArray(&m_precompressedMeta[0]);
            m_precompressor.applyReverse(&m_macroblockIsInter[0], &m_macroblockIsInter[0] + m_macroblockIsInter.size());
            m_precompressor.applyReverse(&m_motionVectorsX[0], &m_motionVectorsX[0] + m_motionVectorsX.size());
            m_precompressor.applyReverse(&m_motionVectorsY[0], &m_motionVectorsY[0] + m_motionVectorsY.size());
            m_previous.fillFrom(m_current.regionBegin(0, 0, m_current.getAlignedWidth(), m_current.getAlignedHeight()),
                                    m_current.regionEnd());
            m_precompressor.setByteArray(&m_precompressed[0]);
            m_prevMacroblockAverage = 128;
            for(Frame<>::coord_t macroblock = 0;
                macroblock < (m_current.getAlignedWidth() * m_current.getAlignedHeight()) / (16 * 16);
                macroblock++)
            {
                m_precompressor.applyReverse(m_current.scanningBegin(m_zigZagScan, macroblock),
                                             m_current.scanningBegin(m_zigZagScan, macroblock + 1));
                processMacroblockReverse(macroblock);
            }
            m_normalize(m_current.horizontalBegin(), m_current.horizontalEnd());
            m_current.toByteArray(&m_uncompressed[0]);
            m_byteArraySerializer.serializeByteArray(&m_uncompressed[0], m_uncompressed.size(), m_output, false);
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

void Codec::processMacroblockForward(Frame<>::coord_t macroblock, bool forceI)
{
    uint32_t sad;
    Frame<>::coord_t currentX = (macroblock % m_macroblockWidth) * 16,
                     currentY = (macroblock / m_macroblockWidth) * 16;
    m_motionEstimator(m_current, m_previous, macroblock, &m_motionVectorsX[macroblock],
                      &m_motionVectorsY[macroblock], &sad);
    if(!forceI && sad < 10 * 16 * 16)
    {
        m_macroblockIsInter[macroblock / 8] |= (1 << (macroblock % 8));
        m_predictor.applyForward(m_current.horizontalBegin(macroblock),
                                 m_current.horizontalBegin(macroblock + 1),
                                 m_previous.regionBegin(currentX, currentY,
                                                        m_motionVectorsX[macroblock],
                                                        m_motionVectorsY[macroblock],
                                                        16, 16));
    }
    else
    {
        m_macroblockIsInter[macroblock / 8] &= ~(1 << (macroblock % 8));
        m_motionVectorsX[macroblock] = 0;
        m_motionVectorsY[macroblock] = 0;
    }
    if(macroblock != 0 &&
       (((m_macroblockIsInter[macroblock / 8] & (1 << (macroblock % 8))) != 0) ==
        ((m_macroblockIsInter[(macroblock - 1) / 8] & (1 << ((macroblock - 1) % 8))) != 0)))
    {
        m_predictor.applyForward(m_current.horizontalBegin(macroblock),
                                 m_current.horizontalBegin(macroblock + 1),
                                 makeConstantValueIterator(m_prevMacroblockAverage));
    }
    else
    {
        if((m_macroblockIsInter[macroblock / 8] & (1 << (macroblock % 8))) == 0)
        {
            m_predictor.applyForward(m_current.horizontalBegin(macroblock),
                                     m_current.horizontalBegin(macroblock + 1),
                                     makeConstantValueIterator(128));
        }
    }
    m_dct.applyForward(m_current.horizontalBegin(macroblock),
                       m_current.horizontalBegin(macroblock + 1));
    m_dct.applyForward(m_current.verticalBegin(macroblock),
                       m_current.verticalBegin(macroblock + 1));
    m_quantization.applyForward(m_current.horizontalBegin(macroblock),
                                m_current.horizontalBegin(macroblock + 1));
}

void Codec::processMacroblockReverse(Frame<>::coord_t macroblock)
{
    Frame<>::coord_t currentX = (macroblock % m_macroblockWidth) * 16,
                     currentY = (macroblock / m_macroblockWidth) * 16;
    m_quantization.applyReverse(m_current.horizontalBegin(macroblock),
                                m_current.horizontalBegin(macroblock + 1));
    m_dct.applyReverse(m_current.horizontalBegin(macroblock),
                       m_current.horizontalBegin(macroblock + 1));
    m_dct.applyReverse(m_current.verticalBegin(macroblock), m_current.verticalBegin(macroblock + 1));
    if(macroblock != 0 &&
       (((m_macroblockIsInter[macroblock / 8] & (1 << (macroblock % 8))) != 0) ==
        ((m_macroblockIsInter[(macroblock - 1) / 8] & (1 << ((macroblock - 1) % 8))) != 0)))
    {
        m_predictor.applyReverse(m_current.horizontalBegin(macroblock),
                                 m_current.horizontalBegin(macroblock + 1),
                                 makeConstantValueIterator(m_prevMacroblockAverage));
    }
    else
    {
        if((m_macroblockIsInter[macroblock / 8] & (1 << (macroblock % 8))) == 0)
        {
            m_predictor.applyReverse(m_current.horizontalBegin(macroblock),
                                     m_current.horizontalBegin(macroblock + 1),
                                     makeConstantValueIterator(128));
        }
    }
    m_prevMacroblockAverage = m_findAverage(m_current.horizontalBegin(macroblock),
                                            m_current.horizontalBegin(macroblock + 1));
    if((m_macroblockIsInter[macroblock / 8] & (1 << (macroblock % 8))) != 0)
    {
        m_predictor.applyReverse(m_current.horizontalBegin(macroblock),
                                 m_current.horizontalBegin(macroblock + 1),
                                 m_previous.regionBegin(currentX, currentY,
                                                        m_motionVectorsX[macroblock],
                                                        m_motionVectorsY[macroblock],
                                                        16, 16));
    }
}

}
