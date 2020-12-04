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

#ifndef _XOODOODCLC_H_
#define _XOODOODCLC_H_

#include <string>
#include "Xoodoo.h"

class XoodooPropagation;
class XoodooPlane;

/** As a utility class for XoodooDCLC, this class lists the output Column patterns
  * (differences or linear masks) compatible with a given Column input pattern
  * through either χ or χ<sup>-1</sup>.
  * The main attribute is @a values, which lists all the corresponding output Column patterns.
  * Associated with each output value (same index in the vector),
  * its propagation weight is in the vector @a weights.
  * Note that the values are sorted in increasing weight order,
  * so one can start looking for the outputs with lowest weight first.
  * The maxWeight and minWeight attributes indicate the maximum and minimum
  * propagation weights present, respectively.
  */
class ListOfColumnPatterns {
public:
	/** The list of output Column patterns (differences or selection vectors)
	  * compatible with a given Column input pattern.
	  */
	vector<ColumnValue> values;
	/** The list of propagation weights, for each Column pattern, i.e.,
	  * weights[i] is the weight of values[i].
	  */
	vector<int> weights;
	/** True iff maxWeight and minWeight are meaningful. */
	bool minMaxInitialized;
	/** The maximum propagation weight in weights.*/
	int maxWeight;
	/** The minimum propagation weight in weights.*/
	int minWeight;
public:
	/** This constructor initializes the output patterns to the empty set.
	  */
	ListOfColumnPatterns() : minMaxInitialized(false) {}
	/** This function adds a possible output pattern,
	  * along with an associated propagation weight,
	  * while inserting it at the right
	  * place to ensure that they are listed with increasing propagation weights.
	  * @param  aValue  An output pattern.
	  * @param  aWeight Its associated propagation weight.
	  */
	void add(ColumnValue aValue, int aWeight);
	/** This function displays the possible patterns and their weights.
	  * @param  fout    The stream to display to.
	  */
	void display(ostream& fout) const;
};

/** This class is an extension of Xoodoo with additional functionality
  * aimed at differential and linear cryptanalysis.
  */
class XoodooDCLC : public Xoodoo {
public:
	/** This attribute contains the output difference patterns for the non-linear function χ.
	   * The index of the vector corresponds to the input difference pattern.
	   */
	vector<ListOfColumnPatterns> diffChi;
	/** This attribute contains the output difference patterns for the non-linear function χ<sup>-1</sup>.
	  * The index of the vector corresponds to the input difference pattern.
	  */
	vector<ListOfColumnPatterns> diffInvChi;
	/** This attribute contains the output linear masks for the non-linear function χ.
	  * The index of the vector corresponds to the input mask.
	  */
	vector<ListOfColumnPatterns> corrChi;
	/** This attribute contains the output linear masks for the non-linear function χ<sup>-1</sup>.
	  * The index of the vector corresponds to the input mask.
	  */
	vector<ListOfColumnPatterns> corrInvChi;
	/** This attribute indicates, for each λ mode (see LambdaMode), if θ is just after χ.
	  * When true, this means that lambdaBeforeTheta() is the identity.
	  */
	vector<bool> thetaJustAfterChi;
	/** This attribute indicates, for each λ mode (see LambdaMode), if θ is just before χ.
	  * When true, this means that lambdaAfterTheta() is the identity.
	  */
	vector<bool> thetaJustBeforeChi;
	
	/** In this context, λ represents the linear operations in Xoodoo
      * between two applications of χ.
      * The λ mode indicates whether to perform these operations, their transpose,
      * their inverse or the transpose of their inverse.
      * - Straight: λ(a) means ρWest(θ(ρEast(a)));
      * - Inverse: λ(a) means ρEast<sup>-1</sup>(θ<sup>-1</sup>(ρEast<sup>-1</sup>(a)));
      * - Transpose: λ(a) means ρEast<sup>-1</sup>(θ<sup>T</sup>(ρEast<sup>-1</sup>(a)));
      * - Dual: λ(a) means ρWest(θ<sup>-1T</sup>(ρEast(a))).
      */
    enum LambdaMode {
        Straight = 0,
        Inverse,
        Transpose,
        Dual,
        EndOfLambdaModes
    };
    XoodooDCLC() = default;
    /** This constructor has the same parameters as Xoodoo::Xoodoo.
      */
    XoodooDCLC(unsigned int aSizeX, unsigned int aSizeZ, const XoodooParameters& aParameters);
    /** This method retuns a string describing the instance. */
    string getDescription() const;

	void displayAll(ostream& fout, XoodooPropagation *DC = 0)const;
    /** Apply the χ function on a single column value.
      * @param   a      The input column value.
      * @return The output column value.
      */
    ColumnValue chiOnColumn(ColumnValue a) const;
    /** Apply the χ<sup>-1</sup> function on a single column value.
      * @param   a      The input column value.
      * @return The output column value.
      */
    ColumnValue inverseChiOnColumn(ColumnValue a) const;
    /** This method applies the λ function (see LambdaMode).
      * The mode argument gives the λ mode (i.e., inverse/transpose) to use.
      * @param   state  The state to apply λ to.
      * @param   mode   The λ mode.
      */
    void lambda(XoodooState& state, LambdaMode mode) const;
    void lambdaBeforeTheta(XoodooState& state, LambdaMode mode) const;
    void lambdaTheta(XoodooState& state, LambdaMode mode) const;
    void lambdaAfterTheta(XoodooState& state, LambdaMode mode) const;
    void lambdaThetaAndAfter(XoodooState& state, LambdaMode mode) const;
    /** Get the parity of a state before or after θ.
     */
    void getParity(const XoodooState& state, XoodooPlane& parity) const;
private:
    void initializeAll();
};

class XoodooPlane : public XoodooLanes {
public:
    const Xoodoo& instance;
    const unsigned int sizeZ;
public:
    XoodooPlane(const Xoodoo& anInstance);
    XoodooPlane& operator=(const XoodooPlane& other);
    inline LaneValue getLane(unsigned int x) const
    {
        return lanes[x];
    }
    /** This method returns the value of a given bit in a state.
      *
      * @param  x   The x coordinate.
      * @param  z   The z coordinate.
      * @return The bit value.
      */
    inline int getBit(unsigned int x, unsigned int z) const
    {
        return (lanes[x] >> z) & 1;
    }
    /** This method sets to 0 a particular bit in a state.
      *
      * @param  x   The x coordinate.
      * @param  z   The z coordinate.
      */
    inline void setBitToZero(unsigned int x, unsigned int z)
    {
        lanes[x] &= ~((LaneValue)1 << z);
    }
    /** This method sets to 1 a particular bit in a state.
      *
      * @param  x   The x coordinate.
      * @param  z   The z coordinate.
      */
    inline void setBitToOne(unsigned int x, unsigned int z)
    {
        lanes[x] |= ((LaneValue)1 << z);
    }
    /** This method inverts a particular bit in a state.
      *
      * @param  x   The x coordinate.
      * @param  z   The z coordinate.
      */
    inline void invertBit(unsigned int x, unsigned int z)
    {
        lanes[x] ^= ((LaneValue)1 << z);
    }
    string getDisplayString(unsigned int x) const;
    void display(ostream& fout) const;
    friend ostream& operator<<(ostream& a, const XoodooPlane& state);
};

#endif
