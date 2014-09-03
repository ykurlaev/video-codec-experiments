#include "format.h"
#include <stdexcept>

namespace Codec
{

using std::runtime_error;

Format::Format(FILE *file, size_t bufferSize)
    : m_file(file), m_precompressedBuffer(bufferSize),
      m_precompressedBufferPtr(&m_precompressedBuffer[0]),
      m_compressedBuffer(bufferSize)
{
    m_zlibCompress.setOutput(&m_compressedBuffer[0], m_compressedBuffer.size());
}

void Format::writeHeader(HeaderParams params)
{
    writeUint32(params.m_width);
    writeUint32(params.m_height);
    writeUint32(params.m_quantizationMode);
    writeUint32(params.m_quality);
}

Format::HeaderParams Format::readHeader()
{
    uint32_t uwidth;
    uint32_t uheight;
    uint32_t umode;
    uint32_t uquality;
    readUint32(uwidth);
    readUint32(uheight);
    readUint32(umode);
    readUint32(uquality);
    return HeaderParams(static_cast<coord_t>(uwidth), static_cast<coord_t>(uheight),
                        static_cast<QuantizationMode>(umode), static_cast<uint8_t>(uquality));
}

void Format::setFrameMode(MacroblockMode frameMode)
{
    uint8_t a[1] = { (frameMode == B) ? 1 : 0 };
    m_zlibCompress(a, 1);
}

void Format::writeMacroblock(uint8_t *buffer, size_t size)
{
    m_zlibCompress(buffer, size);
}

void Format::writeFrame()
{
    size_t compressedSize = m_zlibCompress.getOutputSize();
    m_zlibCompress.setOutput(&m_compressedBuffer[0], m_compressedBuffer.size());
    writeUint32(compressedSize);
    size_t n = fwrite(&m_compressedBuffer[0], 1, compressedSize, m_file);
    if(n != compressedSize)
    {
        throw runtime_error("Can't write output stream");
    }
    m_precompressedBufferPtr = &m_precompressedBuffer[0];
}

bool Format::readFrame()
{
    uint32_t usize;
    if(!readUint32(usize))
    {
        return false;
    }
    size_t size = static_cast<size_t>(usize);
    if(size == 0)
    {
        return false;
    }
    size_t n = fread(&m_compressedBuffer[0], 1, size, m_file);
    if(feof(m_file))
    {
        return false;
    }
    if(n != size)
    {
        throw runtime_error("Can't read input stream");
    }
    m_zlibDecompress(&m_compressedBuffer[0], &m_precompressedBuffer[0],
                     size, m_precompressedBuffer.size());
    m_precompressedBufferPtr = &m_precompressedBuffer[0] + 1;
    return true;
}

Format::MacroblockMode Format::getFrameMode()
{
    return (m_precompressedBuffer[0] == 1) ? B : P;
}

size_t Format::writeMacroblockParams(MacroblockParams params, uint8_t *ptr)
{
    size_t result = 0;
    uint8_t x = static_cast<uint8_t>(params.m_xMotion);
    uint8_t y = static_cast<uint8_t>(params.m_yMotion);
    uint8_t xSign = (((x >> 2) & 0x01) != 0) ? 0xFC : 0x00;
    uint8_t ySign = (((y >> 2) & 0x01) != 0) ? 0xFC : 0x00;
    switch(params.m_mode)
    {
        case I:
            *ptr = params.m_IMode & 0x3F;
            result = 1;
            break;
        case P:
            if(((x & 0xFC) == xSign) && ((y & 0xFC) == ySign))
            {
                *ptr = 0x80 | ((x & 0x07) << 3) | (y & 0x07);
                result = 1;
            }
            else
            {
                *ptr = 0xC0 | ((x & 0x7f) >> 1);
                *(ptr + 1) = ((x & 0x01) << 7) | (y & 0x7F);
                result = 2;
            }
            break;
        case P2:
        case B:
            uint8_t x2 = static_cast<uint8_t>(params.m_xMotion2);
            uint8_t y2 = static_cast<uint8_t>(params.m_yMotion2);
            uint8_t xSign2 = (((x2 >> 2) & 0x01) != 0) ? 0xFC : 0x00;
            uint8_t ySign2 = (((y2 >> 2) & 0x01) != 0) ? 0xFC : 0x00;
            if(((x & 0xFC) == xSign) && ((y & 0xFC) == ySign) &&
               ((x2 & 0xFC) == xSign2) && ((y2 & 0xFC) == ySign2))
            {
                *ptr = ((params.m_mode == P2) ? 0x40 : 0x60) | ((x & 0x07) << 1) | ((y & 0x04) >> 2);
                *(ptr + 1) = ((y & 0x03) << 6) | ((x2 & 0x07) << 3) | (y2 & 0x07);
                result = 2;
            }
            else
            {
                *ptr = ((params.m_mode == P2) ? 0x50 : 0x70) | ((x & 0x78) >> 3);
                *(ptr + 1) = ((x & 0x07) << 5) | ((y & 0x7C) >> 2);
                *(ptr + 2) = ((y & 0x03) << 6) | ((x2 & 0x7E) >> 1);
                *(ptr + 3) = ((x2 & 0x01) << 7) | (y2 & 0x7F);
                result = 4;
            }            
            break;
    }
    return result;
}

size_t Format::readMacroblockParams(MacroblockParams &params, uint8_t *ptr)
{
    size_t result;
    uint8_t first = *ptr;
    uint8_t uVectors[4];
    if((first & 0x80) == 0)
    {
        if((first & 0x40) == 0)
        {
            params.m_mode = I;
            params.m_IMode = first & 0x7F;
            result = 1;
        }
        else
        {
            params.m_mode = ((first & 0x20) == 0) ? P2 : B;
            if((first & 0x10) == 0)
            {
                uint8_t second = *(ptr + 1);
                uVectors[0] = ((((first >> 3) & 0x01) != 0) ? 0xF8 : 0x00) |
                              ((first & 0x0E) >> 1);
                uVectors[1] = (((first & 0x01) != 0) ? 0xF8 : 0x00) | 
                              ((first & 0x01) << 2) | ((second & 0xC0) >> 6);
                uVectors[2] = ((((second >> 5) & 0x01) != 0) ? 0xF8 : 0x00) |
                              ((second & 0x38) >> 3);
                uVectors[3] = ((((second >> 2) & 0x01) != 0) ? 0xF8 : 0x00) |
                              (second & 0x07);
                result = 2;
            }
            else
            {
                uint8_t second = *(ptr + 1);
                uint8_t third = *(ptr + 2);
                uint8_t fourth = *(ptr + 3);
                uVectors[0] = ((((first >> 3) & 0x01) != 0) ? 0x80 : 0x00) |
                              ((first & 0x0F) << 3) | ((second & 0xE0) >> 5);
                uVectors[1] = ((((second >> 4) & 0x01) != 0) ? 0x80 : 0x00) |
                              ((second & 0x1F) << 2) | ((third & 0xC0) >> 6);
                uVectors[2] = ((((third >> 5) & 0x01) != 0) ? 0x80 : 0x00) |
                              ((third & 0x3F) << 1) | ((fourth & 0x80) >> 7);
                uVectors[3] = ((((fourth >> 6) & 0x01) != 0) ? 0x80 : 0x00) |
                              (fourth & 0x7F);
                result = 4;
            }
            params.m_xMotion = static_cast<int8_t>(uVectors[0]);
            params.m_yMotion = static_cast<int8_t>(uVectors[1]);
            params.m_xMotion2 = static_cast<int8_t>(uVectors[2]);
            params.m_yMotion2 = static_cast<int8_t>(uVectors[3]);
        }    
    }
    else
    {
        params.m_mode = P;
        if((first & 0x40) == 0)
        {
            uVectors[0] = ((((first >> 5) & 0x01) != 0) ? 0xF8 : 0x00) |
                          ((first & 0x38) >> 3);
            uVectors[1] = ((((first >> 2) & 0x01) != 0) ? 0xF8 : 0x00) |
                          (first & 0x07);
            result = 1;
        }
        else
        {
            uint8_t second = *(ptr + 1);
            uVectors[0] = ((((first >> 5) & 0x01) != 0) ? 0x80 : 0x00 ) |
                          ((first & 0x3F) << 1) | ((second & 0x80) >> 7);
            uVectors[1] = ((((second >> 6) & 0x01) != 0) ? 0x80 : 0x00 ) |
                          (second & 0x7F);
            result = 2;
        }
        params.m_xMotion = static_cast<int8_t>(uVectors[0]);
        params.m_yMotion = static_cast<int8_t>(uVectors[1]);
    }
    return result;
}

void Format::writeUint32(uint32_t uint32)
{
    uint8_t bytes[4] = {static_cast<uint8_t>(uint32 & 0xFF),
                        static_cast<uint8_t>((uint32 >> 8) & 0xFF),
                        static_cast<uint8_t>((uint32 >> 16) & 0xFF),
                        static_cast<uint8_t>((uint32 >> 24) & 0xFF)};
    size_t n = fwrite(bytes, 1, 4, m_file);
    if(n != 4)
    {
        throw runtime_error("Can't write output stream");
    }
}

bool Format::readUint32(uint32_t &uint32)
{
    uint8_t bytes[4] = {0};
    size_t n = fread(bytes, 1, 4, m_file);
    if(feof(m_file))
    {
        return false;
    }
    if(n != 4)
    {
        throw runtime_error("Can't read input stream");
    }
    uint32 = bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24);
    return true;
}

}
