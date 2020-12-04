/*
KeccakTools

The Keccak sponge function, designed by Guido Bertoni, Joan Daemen,
Michaël Peeters and Gilles Van Assche. For more information, feedback or
questions, please refer to our website: http://keccak.noekeon.org/

Implementation by the designers,
hereby denoted as "the implementer".

To the extent possible under law, the implementer has waived all copyright
and related or neighboring rights to the source code in this file.
http://creativecommons.org/publicdomain/zero/1.0/
*/

#include <iostream>
#include <sstream>
#include "XoodooParts.h"

using namespace std;

SliceValue getSliceValue(ColumnValue Column0, ColumnValue Column1, ColumnValue Column2)
{
	return
		getSliceFromColumn(Column0, 0)
		^ getSliceFromColumn(Column1, 1)
		^ getSliceFromColumn(Column2, 2);
      
}

unsigned int getNrActiveColumns(const SliceValue& slice)
{
	return
		(getColumnFromSlice(slice, 0) != 0 ? 1 : 0)
		+ (getColumnFromSlice(slice, 1) != 0 ? 1 : 0)
		+ (getColumnFromSlice(slice, 2) != 0 ? 1 : 0);
}

unsigned int getNrActiveColumns(const vector<SliceValue>& slices)
{
    unsigned int a = 0;
    for(unsigned int i=0; i<slices.size(); i++)
        a += getNrActiveColumns(slices[i]);
    return a;
}

unsigned int getNrActiveColumns(const vector<LaneValue>& lanes)
{
    unsigned int result = 0;
    for(unsigned int x=0; x<4; x++) {
		LaneValue m = lanes[0 + 3 * x]
			| lanes[1 + 3 * x]
			| lanes[2 + 3 * x];
        
        result += getHammingWeightLane(m);
    }
    return result;
}


ColumnValue translateColumnSafely(ColumnValue Column, int dx)
{
    dx %= 4;
    if (dx < 0) dx += 4;
    return translateColumn(Column, dx);
}

static const unsigned char HammingWeightTable[256] = {
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
};

unsigned int getHammingWeightRow(RowValue Row)
{
    return HammingWeightTable[Row];
}

unsigned int getHammingWeightColumn(ColumnValue column)
{
    return HammingWeightTable[column];
}

SliceValue translateSlice(SliceValue slice, unsigned int dx, unsigned int dy)
{
    return getSliceValue(
        translateColumn(getColumnFromSlice(slice, (4-dx)%4), dy),
        translateColumn(getColumnFromSlice(slice, (5-dx)%4), dy),
        translateColumn(getColumnFromSlice(slice, (6-dx)%4), dy));
}

SliceValue translateSliceSafely(SliceValue slice, int dx, int dy)
{
    dx %= 4;
    if (dx < 0) dx += 4;
    dy %= 3;
    if (dy < 0) dy += 3;
    return translateSlice(slice, dx, dy);
}

unsigned int getHammingWeightSlice(SliceValue slice)
{
	return HammingWeightTable[slice & 0xFF]
		+ HammingWeightTable[(slice >> 8) & 0xFF];
}

unsigned int getHammingWeight(const vector<SliceValue>& state)
{
    unsigned int result = 0;
    for(unsigned int z=0; z<state.size(); z++)
        result += getHammingWeightSlice(state[z]);
    return result;
}

unsigned int getHammingWeightLane(LaneValue lane)
{
	return HammingWeightTable[lane & 0xFF]
		+ HammingWeightTable[(lane >> 8) & 0xFF]
		+ HammingWeightTable[(lane >> 16) & 0xFF]
		+ HammingWeightTable[(lane >> 24) & 0xFF];
      
}

unsigned int getHammingWeight(const vector<LaneValue>& state)
{
    unsigned int result = 0;
    for(unsigned int z=0; z<state.size(); z++)
        result += getHammingWeightLane(state[z]);
    return result;
}

void translateStateAlongZ(vector<SliceValue>& state, unsigned int dz)
{
    if (dz != 0) {
        vector<SliceValue> stateBis(state);
        for(unsigned int z=0; z<stateBis.size(); z++) {
            unsigned int newZ = (z+dz)%stateBis.size();
            state[newZ] = stateBis[z];
        }
    }
}

ColumnValue getColumn(const vector<LaneValue>& lanes, unsigned int x, unsigned int z)
{
    ColumnValue result = 0;
    for(unsigned int y=0; y<3; y++) {
        if ((lanes[y+3*x] & ((LaneValue)1 << z)) != 0)
            result ^= (1 << y);
    }
    return result;
}

void setColumn(vector<LaneValue>& lanes, ColumnValue Column, unsigned int x, unsigned int z)
{
    for(unsigned int y=0; y<3; y++)
        if ((Column & (1 << y)) != 0)
            lanes[y + 3 * x] |= (LaneValue)1 << z;
        else
            lanes[y + 3 * x] &= ~((LaneValue)1 << z);
}

ColumnValue getColumn(const vector<SliceValue>& slices, unsigned int x, unsigned int z)
{
    return getColumnFromSlice(slices[z], x);
}

void setColumn(vector<SliceValue>& slices, ColumnValue Column, unsigned int x, unsigned int z)
{
    slices[z] = (slices[z] & (~getSliceFromColumn(0x7, x))) | getSliceFromColumn(Column, x);
}

RowValue getRow(const vector<SliceValue>& slices, unsigned int y, unsigned int z)
{
    RowValue Row = 0;
    for(unsigned int x=0; x<4; x++)
        Row |= ((slices[z] >> (y+3*x)) & 1) << x;
    return Row;
}

void setRow(vector<SliceValue>& slices, RowValue Row, unsigned int y, unsigned int z)
{
    /*const SliceValue RowMask = (1 << 0) | (1 << 5) | (1 << 10) | (1 << 15) | (1 << 20);
    slices[z] &= ~(RowMask << y);
    for(unsigned int x=0; x<4; x++)
        slices[z] |= ((Row >> x) & 1) << (y + 3 * x);*/
}

void invertRow(vector<SliceValue>& slices, unsigned int x, unsigned int z)
{
    /*const SliceValue RowMask = (1 << 0) | (1 << 5) | (1 << 10) | (1 << 15) | (1 << 20);
    slices[z] ^= (RowMask << x);*/
}

SliceValue getSlice(const vector<LaneValue>& lanes, unsigned int z)
{
    SliceValue result = 0;
    for(unsigned int x=0; x<4; x++)
        result ^= getSliceFromColumn(getColumn(lanes, x, z), x);
    return result;
}

void setSlice(vector<LaneValue>& lanes, SliceValue slice, unsigned int z)
{
    for(unsigned int x=0; x < 4; x++)
        setColumn(lanes, getColumnFromSlice(slice, x), x, z);
}

void fromLanesToSlices(const vector<LaneValue>& lanes, vector<SliceValue>& slices, unsigned int laneSize)
{
    slices.resize(laneSize);
    for(unsigned int z=0; z<laneSize; z++)
        slices[z] = getSlice(lanes, z);
}

void fromSlicesToLanes(const vector<SliceValue>& slices, vector<LaneValue>& lanes)
{
    lanes.assign(12, 0);
    for(unsigned int z=0; z<slices.size(); z++)
        setSlice(lanes, slices[z], z);
}

LaneIndex getLaneIndexSafely(int x, int y)
{
    return getLaneIndex((4+x%4)%4, (3+3%3)%3);
}
