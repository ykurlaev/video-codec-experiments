#ifndef CODEC_FORMAT_H
#define CODEC_FORMAT_H

#include <cassert>
#include <cstdio>
#include <vector>
//#ifndef DISABLE_VERBOSE
//#include <ostream>
//#endif //DISABLE_VERBOSE
#include "precompressor.h"
#include "util.h"
#include "zlibcompress.h"
#include "zlibdecompress.h"

namespace Codec
{

class Format
{
    public:
        enum QuantizationMode { FLAT = 0, JPEG = 1 };
        struct HeaderParams
        {
            HeaderParams(coord_t width = 0, coord_t height = 0,
                         QuantizationMode quantizationMode = FLAT, uint8_t quality = 0)
                : m_width(width), m_height(height),
                  m_quantizationMode(quantizationMode), m_quality(quality)
            {}
            coord_t m_width;
            coord_t m_height;
            QuantizationMode m_quantizationMode;
            uint8_t m_quality;
        };
        enum MacroblockMode { I = 0, P = 1, P2 = 2, B = 3 };
        struct MacroblockParams
        {
            MacroblockMode m_mode;
            uint8_t m_IMode;
            int8_t m_xMotion;
            int8_t m_yMotion;
            int8_t m_xMotion2;
            int8_t m_yMotion2;
        };
        Format(FILE *file, size_t bufferSize);
        void writeHeader(HeaderParams params);
        HeaderParams readHeader();
        template <typename Iterator>
        size_t precompressMacroblock(MacroblockParams params, Iterator begin, Iterator end, uint8_t *buffer);
        void writeMacroblock(uint8_t *buffer, size_t size);
        void writeFrame();
        bool readFrame();
        template <typename Iterator>
        MacroblockParams readMacroblock(Iterator begin, Iterator end);
    private:
        size_t writeMacroblockParams(MacroblockParams params, uint8_t *ptr);
        size_t readMacroblockParams(MacroblockParams &params, uint8_t *ptr);
        void writeUint32(uint32_t uint32);
        bool readUint32(uint32_t &uint32);
        FILE *m_file;
        Precompressor m_precompressor;
        ZlibCompress m_zlibCompress;
        ZlibDecompress m_zlibDecompress;
        std::vector<uint8_t> m_precompressedBuffer;
        uint8_t *m_precompressedBufferPtr;
        std::vector<uint8_t> m_compressedBuffer;
};

//#ifndef DISABLE_VERBOSE
//std::ostream &operator<<(std::ostream_iterator &stream, Format::HeaderParams headerParams);
//#endif //DISABLE_VERBOSE

}

//## template method implementation ##
namespace Codec
{

template <typename Iterator>
size_t Format::precompressMacroblock(MacroblockParams params, Iterator begin, Iterator end, uint8_t *buffer)
{
    size_t metaSize = writeMacroblockParams(params, buffer);
    m_precompressor.setByteArray(buffer + metaSize);
    m_precompressor.applyForward(begin, end);
    return metaSize + m_precompressor.getBytesProcessed();
}

template <typename Iterator>
Format::MacroblockParams Format::readMacroblock(Iterator begin, Iterator end)
{
    MacroblockParams params;
    m_precompressedBufferPtr += readMacroblockParams(params, m_precompressedBufferPtr);
    m_precompressor.setByteArray(m_precompressedBufferPtr);
    m_precompressor.applyReverse(begin, end);
    m_precompressedBufferPtr += m_precompressor.getBytesProcessed();
    return params;
}

}

#endif //CODEC_FORMAT_H