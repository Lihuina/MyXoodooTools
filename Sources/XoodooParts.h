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

#ifndef _XOODOOPARTS_H_
#define _XOODOOPARTS_H_

#include "Xoodoo.h"
#include "XoodooPositions.h"

using namespace std;

// -------------------------------------------------------------
//
// Rows, columns, lanes and slices
//
// -------------------------------------------------------------

/** The number of rows and columns in Keccak-<i>f</i>.
  */
const int nrColumns = 3;

/** The RowValue type is one byte, containing the 5
  * bits of a row, in the least significant bits of the byte.
  */
typedef unsigned char RowValue;

/** The ColumnsValue type is one byte, containing the 5
  * bits of a column, in the least significant bits of the byte.
  */
typedef unsigned char ColumnValue;

/** The SliceValue type is one 32-bit word, containing the 5
  * rows of a slice, each located in 5 bits of this word. The row y is
  * in the bits corresponding to numerical value (0-31)*32^y
  * in the word.
  * See getSliceFromRow() and getRowFromSlice() for more details.
  */
typedef uint16_t SliceValue;

/** This constant indicates the maximum value if one needs to loop through
  * all the possible slice values.
  */
const SliceValue maxSliceValue = 0xFFF;

/** The LaneIndex type codes the x and y coordinates as the single integer x + 5y.
  */
typedef unsigned short LaneIndex;

/** This method returns the lane index from (x,y) coordinates.
  *
  * @param  x   The x coordinate, with 0 ≤ x < 4.
  * @param  y   The y coordinate, with 0 ≤ y < 3.
  */
inline LaneIndex getLaneIndex(unsigned int x, unsigned int y)
{
	return y + Xoodoo::sizeY*x;
}

/** This method returns the lane index from (x,y) coordinates.
  *
  * @param  x   The x coordinate, which can be any signed integer (reduced modulo 5).
  * @param  y   The y coordinate, which can be any signed integer (reduced modulo 5).
  */
LaneIndex getLaneIndexSafely(int x, int y);

/** This method returns the value of a given bit in a state.
  *
  * @param  slices   The state as a vector of slices.
  * @param  x   The x coordinate.
  * @param  y   The y coordinate.
  * @param  z   The z coordinate.
  */
inline int getBit(const vector<SliceValue>& slices, unsigned int x, unsigned int y, unsigned int z)
{
	return (slices[z] >> (y + Xoodoo::sizeY*x)) & 1;
}

/** This method returns the value of a given bit in a state.
  *
  * @param  slices   The state as a vector of slices.
  * @param  p   The x, y, z coordinates.
  */
inline int getBit(const vector<SliceValue>& slices, const BitPosition& p)
{
	return getBit(slices, p.x, p.y, p.z);
}

/** This method sets to 0 a particular bit in a state.
  *
  * @param  slices   The state as a vector of slices.
  * @param  x   The x coordinate.
  * @param  y   The y coordinate.
  * @param  z   The z coordinate.
  */
inline void setBitToZero(vector<SliceValue>& slices, unsigned int x, unsigned int y, unsigned int z)
{
	slices[z] &= ~((SliceValue)1 << (y + Xoodoo::sizeY*x));
}

/** This method sets to 0 a particular bit in a state.
  *
  * @param  slices   The state as a vector of slices.
  * @param  p   The x, y, z coordinates.
  */
inline void setBitToZero(vector<SliceValue>& slices, const BitPosition& p)
{
	return setBitToZero(slices, p.x, p.y, p.z);
}

/** This method sets to 1 a particular bit in a state.
  *
  * @param  slices   The state as a vector of slices.
  * @param  x   The x coordinate.
  * @param  y   The y coordinate.
  * @param  z   The z coordinate.
  */
inline void setBitToOne(vector<SliceValue>& slices, unsigned int x, unsigned int y, unsigned int z)
{
	slices[z] |= (SliceValue)1 << (y + Xoodoo::sizeY*x);
}

/** This method sets to 1 a particular bit in a state.
  *
  * @param  slices   The state as a vector of slices.
  * @param  p   The x, y, z coordinates.
  */
inline void setBitToOne(vector<SliceValue>& slices, const BitPosition& p)
{
	setBitToOne(slices, p.x, p.y, p.z);
}

/** This method inverts a particular bit in a state.
  *
  * @param  slices   The state as a vector of slices.
  * @param  x   The x coordinate.
  * @param  y   The y coordinate.
  * @param  z   The z coordinate.
  */
inline void invertBit(vector<SliceValue>& slices, unsigned int x, unsigned int y, unsigned int z)
{
	slices[z] ^= (SliceValue)1 << (y + Xoodoo::sizeY*x);
}

/** This method inverts a particular bit in a state.
  *
  * @param  slices   The state as a vector of slices.
  * @param  p   The x, y, z coordinates.
  */
inline void invertBit(vector<SliceValue>& slices, const BitPosition& p)
{
	invertBit(slices, p.x, p.y, p.z);
}

/** This function returns a SliceValue with bits set to zero, except at
  * row y, where the value is given by the argument @a row.
  */
inline SliceValue getSliceFromColumn(const ColumnValue& column, const unsigned int& x)
{
	return (SliceValue)column << (3 * x);
}

/** This function returns the row value at row y in the given slice value.
  */
inline ColumnValue getColumnFromSlice(const SliceValue& slice, const unsigned int& x)
{
	return (slice >> (3 * x)) & 0x7;
}

/** This method returns the value of a given bit in a plane.
  *
  * @param  slices   The state as a vector of rows.
  * @param  x   The x coordinate.
  * @param  z   The z coordinate.
  */
inline int getBit(const vector<RowValue>& rows, unsigned int x, unsigned int z)
{
	return (rows[z] >> x) & 1;
}
/** This method returns the value of a given bit in a Sheet.
  *
  * @param    The state as a vector of columns.
  * @param  x   The x coordinate.
  * @param  z   The z coordinate.
  */
inline int getBitfromColumn(const vector<ColumnValue>& columns, unsigned int y, unsigned int z)
{
	return (columns[z] >> y) & 1;
}

/** This method sets to 0 a particular bit in a plane.
  *
  * @param  slices   The state as a vector of rows.
  * @param  x   The x coordinate.
  * @param  z   The z coordinate.
  */
//inline void setBitToZero(vector<RowValue>& rows, unsigned int x, unsigned int z)
//{
//	rows[z] &= ~((RowValue)1 << x);
//}

/** This method sets to 0 a particular bit in a Sheet.
  *
  * @param    The state as a vector of columns.
  * @param  y   The x coordinate.
  * @param  z   The z coordinate.
  */
inline int setColumnBitToZero(vector<ColumnValue>& columns, unsigned int y, unsigned int z)
{
    columns[z] &= ~((ColumnValue)1 << y);
}

/** This method sets to 1 a particular bit in a plane.
  *
  * @param  slices   The state as a vector of rows.
  * @param  x   The x coordinate.
  * @param  z   The z coordinate.
  */
//inline void setBitToOne(vector<RowValue>& rows, unsigned int x, unsigned int z)
//{
//	rows[z] |= (RowValue)1 << x;
//}
/** This method sets to 0 a particular bit in a Sheet.
  *
  * @param    The state as a vector of columns.
  * @param  y   The x coordinate.
  * @param  z   The z coordinate.
  */
inline int setColumnBitToOne(vector<ColumnValue>& columns, unsigned int y, unsigned int z)
{
	columns[z] |= (ColumnValue)1 << y;
}

/** This method returns the value of a given row in a slice.
  *
  * @param  slices   The state as a vector of slices.
  * @param  y   The y coordinate.
  * @param  z   The z coordinate.
  */
//RowValue getRow(const vector<SliceValue>& slices, unsigned int y = 0, unsigned int z = 0);
ColumnValue getColumn(const vector<SliceValue>& slices, unsigned int x = 0, unsigned int z = 0);
/** This method returns the value of a given row in a slice.
  *
  * @param  slices   The state as a vector of slices.
  * @param  p   The y, z coordinates.
  */
//inline RowValue getRow(const vector<SliceValue>& slices, const RowPosition& q)
//{
//	return getRow(slices, q.y, q.z);
//}
/** This method returns the value of a given column in a slice.
  *
  * @param  slices   The state as a vector of slices.
  * @param  p   The x, z coordinates.
  */
inline ColumnValue getColumn(const vector<SliceValue>& slices, const ColumnPosition& p)
{
	return getColumn(slices, p.x, p.z);
}
/** This method sets the value of a particular row in a vector of slices.
  *
  * @param  slices   The state as a vector of slices.
  * @param  row     The row value.
  * @param  y   The y coordinate.
  * @param  z   The z coordinate.
  */
//void setRow(vector<SliceValue>& slices, RowValue row, unsigned int y = 0, unsigned int z = 0);
void setColumn(vector<SliceValue>& slices, ColumnValue column, unsigned int x = 0, unsigned int z = 0);
/** This method sets the value of a particular row in a vector of slices.
  *
  * @param  slices   The state as a vector of slices.
  * @param  row     The row value.
  * @param  p   The y, z coordinates.
  */
//inline void setRow(vector<SliceValue>& slices, RowValue row, const RowPosition& p)
//{
//	setRow(slices, row, p.y, p.z);
//}
inline void setColumn(vector<SliceValue>& slices, ColumnValue column, const ColumnPosition& p)
{
	setColumn(slices, column, p.x, p.z);
}
/** This method constructs a slice value from 5 row values.
  */
//SliceValue getSliceValue(RowValue row0, RowValue row1, RowValue row2);
/** This method constructs a slice value from 5 columns values.
  */
SliceValue getSliceValue(ColumnValue column0, ColumnValue column1, ColumnValue column2);
/** This method returns the value of a given column in a slice.
  *
  * @param  slices   The state as a vector of slices.
  * @param  x   The x coordinate.
  * @param  z   The z coordinate.
  */
RowValue getRow(const vector<SliceValue>& slices, unsigned int y = 0, unsigned int z = 0);

/** This method sets the value of a particular column in a vector of slices.
  *
  * @param  slices   The state as a vector of slices.
  * @param  column   The row value.
  * @param  x   The x coordinate.
  * @param  z   The z coordinate.
  */
void setRow(vector<SliceValue>& slices, RowValue column, unsigned int y = 0, unsigned int z = 0);

/** This method complements all the bits of a particular column in a vector of slices.
  *
  * @param  slices   The state as a vector of slices.
  * @param  x   The x coordinate.
  * @param  z   The z coordinate.
  */
void invertRow(vector<SliceValue>& slices, unsigned int y = 0, unsigned int z = 0);

///** This function translates a row value along the X axis and returns the
//  * translated value. Note that 0 <= @a dx < 5 is required.
//  */
//inline RowValue translateRow(const RowValue& row, const unsigned int& dx)
//{
//	if (dx == 0)
//		return row;
//	else
//		return ((row << dx) | (row >> (4 - dx))) & 0xF;
//}
/** This function translates a column value along the y axis and returns the
  * translated value.
  */
//inline ColumnValue translateColumn(const ColumnValue& column, const int& dy)
//{
//	int ddy = ((dy % (int)Xoodoo::sizeY) + (int)Xoodoo::sizeY) % (int)Xoodoo::sizeY;
//	if (ddy == 0)
//		return column;
//	else
//		return ((column << ddy) | (column >> (Xoodoo::sizeY - ddy))) & ((1 << Xoodoo::sizeY) - 1);
//}
/** Same as translateRow, but any (negative and positive) value of dx is allowed.
  */
ColumnValue translateColumnSafely(ColumnValue column, int dy);

/** This function translates a slice value along the X and Y axes and returns the
  * translated value. Note that 0 <= @a dx < 5 and 0 <= @a dy < 5 are required.
  */
SliceValue translateSlice(SliceValue slice, unsigned int dx, unsigned int dy);

/** Same as translateSlice(), but any (negative and positive) value of dx and dy is
  * allowed.
  */
SliceValue translateSliceSafely(SliceValue slice, int dx, int dy);

/**
  * This function translates the state along the Z axis.
  */
void translateStateAlongZ(vector<SliceValue>& state, unsigned int dz);

/** This method returns the value of a given row in a slice.
  *
  * @param  lanes   The state as a vector of lanes.
  * @param  y   The y coordinate.
  * @param  z   The z coordinate.
  */
//RowValue getRow(const vector<LaneValue>& lanes, unsigned int y = 0, unsigned int z = 0);
ColumnValue getColumn(const vector<LaneValue>& lanes, unsigned int x = 0, unsigned int z = 0);

/** This method sets the value of a particular row in a vector of lanes.
  *
  * @param  lanes   The state as a vector of lanes.
  * @param  row     The row value.
  * @param  y   The y coordinate.
  * @param  z   The z coordinate.
  */
//void setRow(vector<LaneValue>& lanes, RowValue row, unsigned int y = 0, unsigned int z = 0);
void setColumn(vector<LaneValue>& lanes, ColumnValue column, unsigned int x = 0, unsigned int z = 0);
/** This method returns the value of a given slice in a state represented as
  * a vector of lanes.
  *
  * @param  lanes   The state as a vector of lanes.
  * @param  z   The slice index (z coordinate).
  */
SliceValue getSlice(const vector<LaneValue>& lanes, unsigned int z = 0);

/** This method sets the value of a particular slice in a vector of lanes.
  *
  * @param  lanes   The state as a vector of lanes.
  * @param  slice   The slice value.
  * @param  z   The z coordinate.
  */
void setSlice(vector<LaneValue>& lanes, SliceValue slice, unsigned int z = 0);

/** This method creates the value of a state represented as a vector of slices
  * from a state represented as a vector of lanes.
  *
  * @param  slices  The destination for the slices.
  * @param  lanes   The state as a vector of lanes.
  * @param  laneSize    The lane size, which is also the number of slices.
  */
void fromLanesToSlices(const vector<LaneValue>& lanes, vector<SliceValue>& slices, unsigned int laneSize);

/** This method creates the value of a state represented as a vector of lanes
 * from a state represented as a vector of slices.
 *
 * @param  lanes   The destination for the lanes.
 * @param  slices  The state as a vector of slices.
 */
void fromSlicesToLanes(const vector<SliceValue>& slices, vector<LaneValue>& lanes);


// -------------------------------------------------------------
//
// Hamming weight and related
//
// -------------------------------------------------------------

/** This function returns the Hamming weight of the given row value.
  */
unsigned int getHammingWeightRow(RowValue row);

/** This function returns the Hamming weight of the given column value.
  */
unsigned int getHammingWeightColumn(ColumnValue column);

/** This function returns the Hamming weight of the given slice value.
  */
unsigned int getHammingWeightSlice(SliceValue slice);

/** This function returns the Hamming weight of the given state.
  */
unsigned int getHammingWeight(const vector<SliceValue>& state);

/** This function returns the Hamming weight of the given lane.
  */
unsigned int getHammingWeightLane(LaneValue lane);

/** This function returns the Hamming weight of the given state.
  */
unsigned int getHammingWeight(const vector<LaneValue>& state);

/** This method returns the number of active rows in the given slice value.
  */
unsigned int getNrActiveColumns(const SliceValue& slice);

/** This method returns the number of active rows in the state given as slices.
  */
unsigned int getNrActiveColumns(const vector<SliceValue>& slices);

/** This method returns the number of active rows in the state given as lanes.
  */
unsigned int getNrActiveColumns(const vector<LaneValue>& lanes);


#endif


