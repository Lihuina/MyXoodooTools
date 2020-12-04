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

#include <iomanip>
#include "XoodooPositions.h"

BitPosition::BitPosition(unsigned int ax, unsigned int ay, unsigned int az)
	: x(ax), y(ay), z(az)
{
}

BitPosition::BitPosition(const BitPosition& aPoint)
	: x(aPoint.x), y(aPoint.y), z(aPoint.z)
{
}

ostream& operator<<(ostream& fout, const BitPosition& point)
{
	fout << "(" << dec << point.x << "," << point.y << "," << std::setw(2) << point.z << ") ";
	return fout;
}

bool BitPosition::operator==(const BitPosition& otherPoint) const
{
	return (x == otherPoint.x && y == otherPoint.y && z == otherPoint.z);
}

bool BitPosition::operator<(const BitPosition& otherPoint) const
{
	if (x < otherPoint.x) return true;
	else if (x == otherPoint.x) {
		if (y < otherPoint.y) return true;
		else if (y == otherPoint.y) {
			if (z < otherPoint.z) return true;
		}
	}
	return false;
}

bool BitPosition::nextXY()
{
	if (y < 2) {
		y++;
		return true;
	}
	if (x < 3) {
		y = 0;
		x++;
		return true;
	}
	return false;
}

void BitPosition::set(unsigned int ax, unsigned int ay, unsigned int az)
{
	x = ax;
	y = ay;
	z = az;
}

void BitPosition::xTranslate(int dx)
{
	x = ((((int)x + dx) % 4) + 4) % 4;
}

void BitPosition::yTranslate(int dy)
{
	y = ((((int)y + dy) % 3) + 3) % 3;
}


void BitPosition::zTranslate(int dz, unsigned int aLaneSize)
{
	z = ((((int)z + dz) % aLaneSize) + aLaneSize) % aLaneSize;
}

void BitPosition::directRhoWest(){



}

void BitPosition::inverseRhoWest() {}
//void BitPosition::directRhoPi() {
//	unsigned int offset[5][5] = {//[x][y] fashion, my state is [y][x]fashion
//		{0, 36, 3, 41, 18},
//		{1, 44, 10, 45, 2},
//		{62, 6, 43, 15, 61},
//		{28, 55, 25, 21, 56},
//		{27, 20, 39, 8, 14}
//	};
//	unsigned int Z = z;
//	unsigned int X = x;
//	unsigned int Y = y;
//	z = (640 + Z + offset[X][Y]) % 64;
//	x = (0 * X + 1 * Y) % 5;
//	y = (2 * X + 3 * Y) % 5;
//	return;
//}
//
//void BitPosition::inverseRhoPi() {
//	unsigned int offset[5][5] = {//[x][y] fashion, my state is [y][x]fashion
//		{0, 36, 3, 41, 18},
//		{1, 44, 10, 45, 2},
//		{62, 6, 43, 15, 61},
//		{28, 55, 25, 21, 56},
//		{27, 20, 39, 8, 14}
//	};
//	unsigned int X = x;
//	unsigned int Y = y;
//	unsigned int Z = z;
//	x = (1 * X + 3 * Y) % 5;
//	y = (1 * X + 0 * Y) % 5;
//	z = (640 + Z - offset[x][y]) % 64;
//	return;
//}

bool operator<(const ColumnPosition& aCP, const ColumnPosition& bCP)
{
	return (aCP.getXplus4Z() < bCP.getXplus4Z());
}

ostream& operator<<(ostream& fout, const ColumnPosition& aCP)
{
	fout << "(" << aCP.x << ",-," << aCP.z << ") ";
	return fout;
}

bool operator<(const RowPosition& aRP, const RowPosition& bRP)
{
	return (aRP.getYplus3Z() < bRP.getYplus3Z());
}

ostream& operator<<(ostream& fout, const RowPosition& aRP)
{
	fout << "(-," << aRP.y << "," << aRP.z << ") ";
	return fout;
}
