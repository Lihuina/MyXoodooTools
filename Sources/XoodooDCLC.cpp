/*
XooTools, a set of C++ classes for analyzing Xoodoo.

Xoodoo, designed by Joan Daemen, Seth Hoffert, Gilles Van Assche and Ronny Van Keer.
For specifications, please refer to https://eprint.iacr.org/2018/767
For contact information, please visit https://keccak.team/team.html

Implementation by Gilles Van Assche, hereby denoted as "the implementer".

To the extent possible under law, the implementer has waived all copyright
and related or neighboring rights to the source code in this file.
http://creativecommons.org/publicdomain/zero/1.0/
*/

#include <algorithm>
#include <fstream>
#include <iostream>
#include <math.h>
#include <sstream>
#include "XoodooDCLC.h"
#include "XoodooPropagation.h"


using namespace std;

const unsigned int nrColumns = Xoodoo::sizeY;
// -------------------------------------------------------------
//
// ListOfColumnPatterns
//
// -------------------------------------------------------------

void ListOfColumnPatterns::add(ColumnValue aValue, int aWeight)
{
	unsigned int i = 0;
	while ((i < values.size()) && (weights[i] <= aWeight))
		i++;
	if (i < values.size()) {
		values.insert(values.begin() + i, aValue);
		weights.insert(weights.begin() + i, aWeight);
	}
	else {
		values.push_back(aValue);
		weights.push_back(aWeight);
	}
	if ((!minMaxInitialized) || (aWeight > maxWeight))
		maxWeight = aWeight;
	if ((!minMaxInitialized) || (aWeight < minWeight))
		minWeight = aWeight;
	minMaxInitialized = true;
}

void ListOfColumnPatterns::display(ostream& fout) const
{
	for (unsigned int i = 0; i < values.size(); i++) {
		fout << dec << weights[i] << "  ";
		fout.fill('0'); fout.width(2); fout << hex << (int)values[i] << endl;
	}
}


// -------------------------------------------------------------
//
// XoodooDCLC
//
// -------------------------------------------------------------


XoodooDCLC::XoodooDCLC(unsigned int aSizeX, unsigned int aSizeZ, const XoodooParameters& aParameters)
    : Xoodoo(aSizeX, aSizeZ, aParameters)
{
	initializeAll();
}

string XoodooDCLC::getDescription() const
{
    return "DC analysis of " + Xoodoo::getDescription();
}


ColumnValue XoodooDCLC::chiOnColumn(ColumnValue a) const
{
    XoodooState state(*this);
    state.setColumn(0, 0, a);
    chi(state);
    return state.getColumn(0, 0);
}

ColumnValue XoodooDCLC::inverseChiOnColumn(ColumnValue a) const
{
    XoodooState state(*this);
    state.setColumn(0, 0, a);
    inverseChi(state);
    return state.getColumn(0, 0);
}

template<class T>
unsigned int dotProduct(const T& a, const T& b)
{
	T aandb = a & b;
	unsigned int result = 0;
	while (aandb != 0) {
		result ^= (aandb & 1);
		aandb >>= 1;
	}
	return result;
}

template<class T>
unsigned int dotProduct(const vector<T>& a, const vector<T>& b)
{
	unsigned int result = 0;
	for (unsigned int i = 0; (i < a.size()) && (i < b.size()); ++i)
		result ^= dotProduct(a[i], b[i]);
	return result;
}

int computeDifferentialWeight(int count)
{
	return (int)floor(nrColumns - log(fabs((double)count)) / log(2.0) + 0.5);
}

int computeLinearWeight(int correl)
{
	return 2 * (int)floor(nrColumns - log(fabs((double)correl)) / log(2.0) + 0.5);
}

void XoodooDCLC::initializeAll()
{
	for (ColumnValue da = 0; da < (1 << nrColumns); da++) {
		vector<unsigned int> count(1 << nrColumns, 0);
		for (ColumnValue a = 0; a < (1 << nrColumns); a++) {
			ColumnValue db = chiOnColumn(a) ^ chiOnColumn(a^da);
			count[db]++;
		}
		ListOfColumnPatterns l;
		for (ColumnValue db = 0; db < (1 << nrColumns); db++)
			if (count[db] != 0)
				l.add(db, computeDifferentialWeight(count[db]));
		diffChi.push_back(l);
	}
	for (ColumnValue da = 0; da < (1 << nrColumns); da++) {
		vector<unsigned int> count(1 << nrColumns, 0);
		for (ColumnValue a = 0; a < (1 << nrColumns); a++) {
			ColumnValue db = inverseChiOnColumn(a) ^ inverseChiOnColumn(a^da);
			count[db]++;
		}
		ListOfColumnPatterns l;
		for (ColumnValue db = 0; db < (1 << nrColumns); db++)
			if (count[db] != 0)
				l.add(db, computeDifferentialWeight(count[db]));
		diffInvChi.push_back(l);
	}
	for (ColumnValue ua = 0; ua < (1 << nrColumns); ua++) {
		vector<int> correl(1 << nrColumns, 0);
		for (ColumnValue ub = 0; ub < (1 << nrColumns); ub++) {
			vector<unsigned int> count(2, 0);
			for (ColumnValue a = 0; a < (1 << nrColumns); a++) {
				ColumnValue b = chiOnColumn(a);
				count[dotProduct(a, ua) ^ dotProduct(b, ub)]++;
			}
			correl[ub] = count[0] - count[1];
		}
		ListOfColumnPatterns l;
		for (ColumnValue ub = 0; ub < (1 << nrColumns); ub++)
			if (correl[ub] != 0)
				l.add(ub, computeLinearWeight(correl[ub]));
		corrChi.push_back(l);
	}
	for (ColumnValue ua = 0; ua < (1 << nrColumns); ua++) {
		vector<int> correl(1 << nrColumns, 0);
		for (ColumnValue ub = 0; ub < (1 << nrColumns); ub++) {
			vector<unsigned int> count(2, 0);
			for (ColumnValue a = 0; a < (1 << nrColumns); a++) {
				ColumnValue b = inverseChiOnColumn(a);
				count[dotProduct(a, ua) ^ dotProduct(b, ub)]++;
			}
			correl[ub] = count[0] - count[1];
		}
		ListOfColumnPatterns l;
		for (ColumnValue ub = 0; ub < (1 << nrColumns); ub++)
			if (correl[ub] != 0)
				l.add(ub, computeLinearWeight(correl[ub]));
		corrInvChi.push_back(l);
	}
	//initializeLambdaLookupTables();
}

void XoodooDCLC::displayAll(ostream& fout, XoodooPropagation *DC)const
{
	for (ColumnValue i = 0; i < (1 << nrColumns); i++) {
		fout << "Difference of \xCF\x87 in one Column: ";
		fout.fill('0'); fout.width(2); fout << hex << (int)i << endl;
		diffChi[i].display(fout);
		if (DC) {
			fout << "Affine description: ";
			DC->affinePerInput[i].display(fout);
			fout << endl;
		}
	}
	for (ColumnValue i = 0; i < (1 << nrColumns); i++) {
		fout << "Difference of \xCF\x87^-1 in one Column: ";
		fout.fill('0'); fout.width(2); fout << hex << (int)i << endl;
		diffInvChi[i].display(fout);
		fout << endl;
	}
	
}

void XoodooDCLC::lambda(XoodooState& state, LambdaMode mode) const
{
    if (mode == Straight) {
        rhoEast(state);
        theta(state);
        rhoWest(state);
    }
    else if (mode == Inverse) {
        inverseRhoWest(state);
        inverseTheta(state);
        inverseRhoEast(state);
    }
    else if (mode == Transpose) {
        inverseRhoWest(state);
        thetaTransposed(state);
        inverseRhoEast(state);
    }
    else if (mode == Dual) {
        rhoEast(state);
        inverseThetaTransposed(state);
        rhoWest(state);
    }
}

void XoodooDCLC::lambdaBeforeTheta(XoodooState& state, LambdaMode mode) const
{
    if (mode == Straight) {
        rhoEast(state);
    }
    else if (mode == Inverse) {
        inverseRhoWest(state);
    }
    else if (mode == Transpose) {
        inverseRhoWest(state);
    }
    else if (mode == Dual) {
        rhoEast(state);
    }
}

void XoodooDCLC::lambdaTheta(XoodooState& state, LambdaMode mode) const
{
    (void)state;
    if (mode == Straight) {
        theta(state);
    }
    else if (mode == Inverse) {
        inverseTheta(state);
    }
    else if (mode == Transpose) {
        thetaTransposed(state);
    }
    else if (mode == Dual) {
        inverseThetaTransposed(state);
    }
}

void XoodooDCLC::lambdaAfterTheta(XoodooState& state, LambdaMode mode) const
{
    if (mode == Straight) {
        rhoWest(state);
    }
    else if (mode == Inverse) {
        inverseRhoEast(state);
    }
    else if (mode == Transpose) {
        inverseRhoEast(state);
    }
    else if (mode == Dual) {
        rhoWest(state);
    }
}

void XoodooDCLC::lambdaThetaAndAfter(XoodooState& state, LambdaMode mode) const
{
    lambdaTheta(state, mode);
    lambdaAfterTheta(state, mode);
}

void XoodooDCLC::getParity(const XoodooState& state, XoodooPlane& parity) const
{
    parity.clear();
    for(unsigned int x=0; x<sizeX; x++)
        parity.getLanes()[x] = state.getLane(x, 0) ^ state.getLane(x, 1) ^ state.getLane(x, 2);
}

XoodooPlane::XoodooPlane(const Xoodoo& anInstance)
    : XoodooLanes(anInstance.getSizeX()), instance(anInstance), sizeZ(anInstance.getSizeZ())
{
}

XoodooPlane& XoodooPlane::operator=(const XoodooPlane& other)
{
    lanes = other.lanes;
    return *this;
}

string XoodooPlane::getDisplayString(unsigned int x) const
{
    const char character[2] = {'.', 'o'};
    string result;
    for(unsigned int z=0; z<instance.getSizeZ(); z++)
        result += character[getBit(x, z)];
    return result;
}

void XoodooPlane::display(ostream& fout) const
{
    for(unsigned int x=0; x<instance.getSizeX(); x++)
        fout << getDisplayString(x) << endl;
}

ostream& operator<<(ostream& a, const XoodooPlane& state)
{
    state.display(a);
    return a;
}
