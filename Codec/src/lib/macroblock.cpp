#include "macroblock.h"
#include "constantvalueiterator.h"
#include "linearinterpolatingiterator.h"

#include <iostream>

namespace Codec
{

Context::Context()
    : m_scan(ZigZagScan<8, SIZE>::getScan())
{ }

Macroblock::Macroblock(Frame<SIZE> *frame, std::vector<Frame<SIZE> *> previousFrames,
                       coord_t number, Macroblock **neighbors,
                       Codec::Context *context)
    : m_frame(frame), m_previousFrames(previousFrames),
      m_number(number), m_context(context)
{
    coord_t macroblockWidth = m_frame->getAlignedWidth() / SIZE;
    m_x = m_number % macroblockWidth;
    m_y = m_number / macroblockWidth;
    for(size_t i = 0; i < 4; i++)
    {
        m_neighbors[i] = neighbors[i];
    }
}

void Macroblock::processForward(Format::MacroblockMode mode)
{
    m_params.m_mode = mode;
    if(m_params.m_mode == Format::P)
    {
        m_context->m_motionEstimator(*m_frame, *m_previousFrames[0], m_number,
                                     &m_params.m_xMotion, &m_params.m_yMotion);
        m_context->m_predictor.applyForward(m_frame->horizontalBegin(m_number),
                                            m_frame->horizontalBegin(m_number + 1),
                                            m_previousFrames[0]->regionBegin(m_x * SIZE + m_params.m_xMotion,
                                                                             m_y * SIZE + m_params.m_yMotion,
                                                                             SIZE, SIZE));
    }
    else if(m_params.m_mode == Format::I)
    {
        m_params.m_IMode = 0;
    }
    else
    {
        m_context->m_motionEstimator(*m_frame, *m_previousFrames[0], m_number,
                                     &m_params.m_xMotion, &m_params.m_yMotion);
        m_context->m_motionEstimator(*m_frame, *m_previousFrames[1], m_number,
                                     &m_params.m_xMotion2, &m_params.m_yMotion2);
        m_context->m_predictor.applyForward(m_frame->horizontalBegin(m_number),
                                            m_frame->horizontalBegin(m_number + 1),
                                            makeLinearInterpolatingIterator(
                                                m_previousFrames[0]->regionBegin(m_x * SIZE + m_params.m_xMotion,
                                                                                 m_y * SIZE + m_params.m_yMotion,
                                                                                 SIZE, SIZE),
                                                m_previousFrames[1]->regionBegin(m_x * SIZE + m_params.m_xMotion2,
                                                                                 m_y * SIZE + m_params.m_yMotion2,
                                                                                 SIZE, SIZE),
                                                -1, (m_params.m_mode == Format::P2) ? -2 : 1));
    }
    data_t predictedAverage = (m_neighbors[0] && m_neighbors[0]->m_params.m_mode == m_params.m_mode) ?
                              m_neighbors[0]->m_average : (m_params.m_mode == Format::I ? 128 : 0);
    m_context->m_predictor.applyForward(m_frame->horizontalBegin(m_number),
                                        m_frame->horizontalBegin(m_number + 1),
                                        makeConstantValueIterator(predictedAverage));
    m_context->m_dct.applyForward(m_frame->horizontalBegin(m_number),
                                  m_frame->horizontalBegin(m_number + 1));
    m_context->m_dct.applyForward(m_frame->verticalBegin(m_number),
                                  m_frame->verticalBegin(m_number + 1));
    m_context->m_quantization.applyForward(m_frame->horizontalBegin(m_number),
                                  m_frame->horizontalBegin(m_number + 1));
}

size_t Macroblock::precompressTo(Format *format, uint8_t *buffer)
{
    return format->precompressMacroblock(m_params, m_frame->scanningBegin(m_context->m_scan, m_number),
                                         m_frame->scanningBegin(m_context->m_scan, m_number + 1),
                                         buffer);
}

void Macroblock::readFrom(Format *format)
{
    m_params = format->readMacroblock(m_frame->scanningBegin(m_context->m_scan, m_number),
                                      m_frame->scanningBegin(m_context->m_scan, m_number + 1));
}

void Macroblock::processReverse()
{
    m_context->m_quantization.applyReverse(m_frame->horizontalBegin(m_number),
                                           m_frame->horizontalBegin(m_number + 1));
    m_context->m_dct.applyReverse(m_frame->horizontalBegin(m_number),
                                  m_frame->horizontalBegin(m_number + 1));
    m_context->m_dct.applyReverse(m_frame->verticalBegin(m_number),
                                  m_frame->verticalBegin(m_number + 1));
    data_t predictedAverage = (m_neighbors[0] && m_neighbors[0]->m_params.m_mode == m_params.m_mode) ?
                              m_neighbors[0]->m_average : (m_params.m_mode == Format::I ? 128 : 0);
    m_context->m_predictor.applyReverse(m_frame->horizontalBegin(m_number),
                                        m_frame->horizontalBegin(m_number + 1),
                                        makeConstantValueIterator(predictedAverage));
    if(m_params.m_mode == Format::I)
    {
        m_IAverage = m_context->m_findAverage(m_frame->horizontalBegin(m_number),
                                              m_frame->horizontalBegin(m_number + 1));
        m_average = m_IAverage;
    }
    else
    {
        ((m_params.m_mode == Format::P) ? m_PAverage : m_P2Average) =
            m_context->m_findAverage(m_frame->horizontalBegin(m_number),
                                     m_frame->horizontalBegin(m_number + 1));
        m_average = (m_params.m_mode == Format::P) ? m_PAverage : m_P2Average;
        if(m_params.m_mode == Format::P)
        {
            m_context->m_predictor.applyReverse(m_frame->horizontalBegin(m_number),
                                                m_frame->horizontalBegin(m_number + 1),
                                                m_previousFrames[0]->regionBegin(m_x * SIZE + m_params.m_xMotion,
                                                                                 m_y * SIZE + m_params.m_yMotion,
                                                                                 SIZE, SIZE));
        }
        else
        {
            m_context->m_predictor.applyReverse(m_frame->horizontalBegin(m_number),
                                                m_frame->horizontalBegin(m_number + 1),
                                                makeLinearInterpolatingIterator(
                                                    m_previousFrames[0]->regionBegin(m_x * SIZE + m_params.m_xMotion,
                                                                                     m_y * SIZE + m_params.m_yMotion,
                                                                                     SIZE, SIZE),
                                                    m_previousFrames[1]->regionBegin(m_x * SIZE + m_params.m_xMotion2,
                                                                                     m_y * SIZE + m_params.m_yMotion2,
                                                                                     SIZE, SIZE),
                                                    -1, (m_params.m_mode == Format::P2) ? -2 : 1));
        }
    }
    m_context->m_normalize(m_frame->horizontalBegin(m_number), m_frame->horizontalBegin(m_number + 1));
}

void Macroblock::chooseMode(Format::MacroblockMode mode)
{
    m_params.m_mode = mode;
    m_average = (m_params.m_mode == Format::I) ? m_IAverage :
                    ((m_params.m_mode == Format::P) ? m_PAverage : m_P2Average);
}

}
