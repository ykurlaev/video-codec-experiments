#include <cstdlib>
#include <algorithm>
#include "findsad.h"

namespace Codec
{

template <uint32_t MIN_N, uint32_t MAX_N>
inline void MotionEstimator::operator()(Frame<MIN_N, MAX_N> &current, Frame<MIN_N, MAX_N> &previous,
                                        MotionEstimator::coord_t macroblock, int8_t *x, int8_t *y, uint32_t *sad)
{
    coord_t macroblockWidth = current.getAlignedWidth() / MAX_N;
    coord_t macroblockHeight = current.getAlignedHeight() / MAX_N;
    coord_t macroblockX = macroblock % macroblockWidth,
            macroblockY = macroblock / macroblockWidth;
    //bool hasNeighbor[9] =
    //    { macroblockX > 0 && macroblockY > 0, macroblockY > 0,
    //          macroblockX < macroblockWidth - 1 && macroblockY > 0,
    //      macroblockX > 0, true, macroblockX < macroblockWidth - 1,
    //      macroblockX > 0 && macroblockY < macroblockHeight - 1, macroblockY < macroblockHeight - 1,
    //          macroblockX < macroblockWidth - 1 && macroblockY < macroblockHeight - 1 };
    //scoord_t neighborOffset[9] = { -1 - macroblockWidth, 0 - macroblockWidth, 1 - macroblockWidth,
    //                               -1, 0, 1,
    //                               -1 + macroblockWidth, macroblockWidth, 1 + macroblockWidth };
    //uint32_t sads[9];
    //std::fill(sads, sads + 9, 0xFFFFFFFF);
    //FindSAD findSAD;
    //for(size_t i = 0; i < 9; i++)
    //{
    //    if(hasNeighbor[i])
    //    {
    //        sads[i] = findSAD(current.horizontalBegin(block), current.horizontalBegin(block + blocksInMacro),
    //                          previous.horizontalBegin(block + neighborOffset[i] * blocksInMacro));
    //    }
    //}
    //uint32_t *sadptr = std::min_element(sads, sads + 9);
    //if(sads[4] <= *sadptr)
    //{
    //    sadptr = &sads[4];
    //}
    //coord_t prevBlock = block + neighborOffset[sadptr - sads] * blocksInMacro;
    //scoord_t prevX = ((prevBlock / blocksInMacro) % macroblockWidth) * MAX_N,
    //         prevY = ((prevBlock / blocksInMacro) / macroblockWidth) * MAX_N;
    //uint32_t prevSad = *sadptr, newSad = prevSad;
    FindSAD findSAD;
    uint32_t prevSad = findSAD(current.horizontalBegin(macroblock), current.horizontalBegin(macroblock + 1),
                               previous.horizontalBegin(macroblock));
    uint32_t newSad = prevSad;
    scoord_t prevX = (macroblock % macroblockWidth) * MAX_N,
             prevY = (macroblock / macroblockWidth) * MAX_N;
    do
    {
        prevX++;
        if(static_cast<coord_t>(prevX) + MAX_N >= macroblockWidth * MAX_N
           || std::abs(static_cast<coord_t>(prevX) - macroblockX * MAX_N) >= 16)
        {
            break;
        }
        prevSad = newSad;
        newSad = findSAD(current.horizontalBegin(macroblock), current.horizontalBegin(macroblock + 1),
                         previous.regionBegin(prevX, prevY, MAX_N, MAX_N));
    }
    while(newSad < prevSad);
    newSad = prevSad;
    prevX--;
    do
    {
        prevX--;
        if(prevX < 0 || std::abs(static_cast<coord_t>(prevX) - macroblockX * MAX_N) >= 16)
        {
            break;
        }
        prevSad = newSad;
        newSad = findSAD(current.horizontalBegin(macroblock), current.horizontalBegin(macroblock + 1),
                         previous.regionBegin(prevX, prevY, MAX_N, MAX_N));
    }
    while(newSad < prevSad);
    newSad = prevSad;
    prevX++;
    do
    {
        prevY++;
        if(static_cast<coord_t>(prevY) + MAX_N >= macroblockHeight * MAX_N
           || std::abs(static_cast<coord_t>(prevY) - macroblockX * MAX_N) >= 16)
        {
            break;
        }
        prevSad = newSad;
        newSad = findSAD(current.horizontalBegin(macroblock), current.horizontalBegin(macroblock + 1),
                         previous.regionBegin(prevX, prevY, MAX_N, MAX_N));
    }
    while(newSad < prevSad);
    newSad = prevSad;
    prevY--;
    do
    {
        prevY--;
        if(prevY < 0 || std::abs(static_cast<coord_t>(prevY) - macroblockX * MAX_N) >= 16)
        {
            break;
        }
        prevSad = newSad;
        newSad = findSAD(current.horizontalBegin(macroblock), current.horizontalBegin(macroblock + 1),
                         previous.regionBegin(prevX, prevY, MAX_N, MAX_N));
    }
    while(newSad < prevSad);
    newSad = prevSad;
    prevY++;
    *x = static_cast<int8_t>(prevX - macroblockX * MAX_N),
    *y = static_cast<int8_t>(prevY - macroblockY * MAX_N), *sad = prevSad;
}

}
