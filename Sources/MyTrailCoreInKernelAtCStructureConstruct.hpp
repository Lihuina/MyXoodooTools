//
//  MyTrailCoreInKernelAtCStructureConstruct.hpp
//  InKernelAtCTrailCoreSearch
//
//  Created by 刘国珍 on 23/05/2018.
//  Copyright © 2018 Liu Guozhen. All rights reserved.
//

#ifndef MyTrailCoreInKernelAtCStructureConstruct_hpp
#define MyTrailCoreInKernelAtCStructureConstruct_hpp

#include <iostream>
#include <cstdlib>
#include <vector>
#include <set>
#include <map>
#include <bitset>
#include <iterator>

#include "Xoodoo.h"
#include "XoodooDCLC.h"
#include "XoodooPropagation.h"
#include "XoodooPositions.h"
#include "types.h"

#include "XoodooState.h"
#include "translationsymmetry.h"
using namespace std;

//typedef unsigned char RowValue;
//typedef UINT32 SliceValue;
typedef std::vector<std::vector<ColumnValue>> DualVector;
typedef std::vector<std::vector<std::vector<ColumnValue>>> TriVector;
typedef multiset <ColumnValue, greater <ColumnValue>> ValuePatternmultiset;
typedef set <ValuePatternmultiset> multisetSet;

struct StartingKnotInfo{
    //    bool isStartingKnot;
    vector<BitPosition> knotPoints;
    unsigned int orbitalMark;
//    vector<unsigned int> yOffset;
};

class InKernelAtCStructureConstruct: public XoodooPropagation{
public:
    InKernelAtCStructureConstruct(unsigned int aMaxWeight, const XoodooDCLC & aParent, XoodooPropagation::DCorLC aDCorLC);
    void genInputforAllActiveSbox(DualVector slicePatternCase,TriVector& activeSboxAllCase);
    void printValueSlicePattern(multisetSet valuePattern);
	void oneSboxValuePatternSearch(TriVector activeSboxAllCase, int DDTOutput[8][8], multisetSet& valuePattern1);
    void twoSboxValuePatternSearch(TriVector activeSboxAllCase, int DDTOutput[8][8], multisetSet& valuePattern2);
    void threeSboxValuePatternSearch(TriVector activeSboxAllCase, int DDTOutput[8][8], multisetSet& valuePattern3);
    void fourSboxValuePatternSearch(TriVector activeSboxAllCase, int DDTOutput[8][8], multisetSet& valuePattern4);
    //void fiveSboxValuePatternSearch(TriVector activeSboxAllCase, int DDTOutput[8][8], multisetSet& valuePattern5);
    void generateValidSliceValuePattern(unsigned int nBit, multisetSet& patternSet);
    void initializeValuePatterns(void);
    void printSingleValuePattern_ForTest(const vector<ColumnValue> aColumnChainOfValuePattern);
    unsigned int Sbox(unsigned int a);
    void differential_distribution_table_and_rhoeast_dir(int DDT[8][8]);

    void getAllSlicePatternsFromAValuePattern(const ValuePatternmultiset aValuePattern, set<SliceValue> & aAllSlicePatternSet);
    void getKnotPointsFromSlice(const SliceValue aSlice, vector<BitPosition>& aStartingKnotPoints);
    void getKnotPointsFromSlice(const SliceValue aSlice, const unsigned int nrSlice, vector<BitPosition>& knotPoints);
    void addPeerParameterOnRho(vector<BitPosition> aPeerAtA);
    bool recursiveYOffsetOfStartingKnot(vector<unsigned int>& yOffset);
    bool updateStartingKnot();
    bool updateStartingKnot(const unsigned int chooseCode);
    void completeStructureOfStartingKnot();
    bool nextStructureOfStaringKnot();
    void completeStructureOfOtherKnot();
    bool nextStructureOfOtherKnot();

    bool nextStructureOfOtherKnot433(BitPosition& aP5B);
    bool nextStructureOfOtherKnot4222();
    void completeStructureOfOtherKnot4222();
    bool nextStructureOfOtherKnot3322A();
    void completeStructureOfOtherKnot3322A();
    bool nextStructureOfOtherKnot3322B();
    void completeStructureOfOtherKnot3322B();
    bool nextStructureOfOtherKnot3322C(BitPosition & Q4B, const BitPosition Q1B);
    BitPosition getBitAtBFromBitAtA(BitPosition pA);
    void completeStructureOfOtherKnot3322C(const BitPosition Q4B);
    bool checkMappedToSameOrbitalCase3322C();
    bool checkMappedToSameKnotAtBCase433(vector<vector<BitPosition>>& aKnotAtB);
    bool checkMappedToSameOrbitalCase4222();
    bool checkMappedToSameKnotCase3322A();
    bool checkKnotInKernelAtC(vector<BitPosition> aKnot);
    bool valuePatternInKernelAtCCheck(const unsigned nrBits, const ValuePatternmultiset aValuePattern);
    ValuePatternmultiset  getValuePatternFromSlice(const SliceValue aSliceValue);
    SliceValue getSliceValueFromBitChain(vector<BitPosition> aBitPointsChain);

protected:
    /** Store all the value patterns of 2, 3, 4, 5 bits.
     */
    multisetSet valuePatternSet2;
    multisetSet valuePatternSet3;
    multisetSet valuePatternSet4;
    multisetSet valuePatternSet5;

    unsigned int maxWeight;
    unsigned int startingKnotUpdateCounter;
    vector<unsigned int> peerParameterOnRho;

    vector<vector<BitPosition>> peersAtA;
//    StartingKnotInfo startingKnot;
    vector<vector<unsigned int>> yOffset;
    vector<BitPosition> startingKnotPoints;
    vector<vector<BitPosition>> knotAtB433;



    bool isLastStructureOfStartingKnot;
    bool isLastStructureOfOtherKnot;
    bool isInitialStructure;
    bool isInitialStructureOfOtherKnot;

    bool isP5Found;
    bool isQ4Found;

	const DualVector sbox1XPosition{
	   {0}, {1}, {2}, {3}
	};
    const DualVector sbox2XPosition {
        {0, 1}, {0, 2}, {0, 3}, {1, 0}, {1, 2}, {1, 3}, 
	    {2, 0}, {2, 1}, {2, 3}, {3, 0}, {3, 1}, {3, 2}
    };
    const DualVector sbox3XPosition {
        {0, 1, 2}, {0, 1, 3}, {0, 2, 1}, {0, 2, 3}, {0, 3, 1}, {0, 3, 2}, 
		{1, 0, 2}, {1, 0, 3}, {1, 2, 0}, {1, 2, 3}, {1, 3, 0}, {1, 3, 2},
	    {2, 0, 1}, {2, 0, 3}, {2, 1, 0}, {2, 1, 3}, {2, 3, 0}, {2, 3, 1},
		{3, 0, 1}, {3, 0, 2}, {3, 1, 0}, {3, 1, 2}, {3, 2, 0}, {3, 2, 1},
        
    };
    const DualVector sbox4XPosition {
        {0, 1, 2, 3}, {0, 1, 3, 2}, {0, 2, 1, 3}, {0, 2, 3, 1}, {0, 3, 1, 2}, {0, 3, 2, 1},
        {1, 0, 2, 3}, {1, 0, 3, 2}, {1, 2, 0, 3}, {1, 2, 3, 0}, {1, 3, 0, 2}, {1, 3, 2, 0},
        {2, 0, 1, 3}, {2, 0, 3, 1}, {2, 1, 0, 3}, {2, 1, 3, 0}, {2, 3, 0, 1}, {2, 3, 1, 0},
        {3, 0, 1, 2}, {3, 0, 2, 1}, {3, 1, 0, 2}, {3, 1, 2, 0}, {3, 2, 0, 1}, {3, 2, 1, 0}
        
    };
    
};

#endif /* MyTrailCoreInKernelAtCStructureConstruct_hpp */
