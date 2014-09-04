/*
 * BigInteger.h
 *
 *  Created on: Apr 2, 2012
 *      Author: nucsoft
 */

#pragma once

#include <vector>
#include <cstdlib>
#include <string>
#include "HelperFunctions.h"

namespace Utilities
{
#define BIGINT_BASETYPE unsigned char

class BigInteger
{
public:
	BigInteger ();
	BigInteger (int number);
	BigInteger (long number);
	BigInteger (const BigInteger& copy);
	virtual ~BigInteger ();

  //BigInteger& getAbs ();

  template <typename T>
  void setFromNumber (T number);
  void setFromString (std::string numberString);

  BigInteger& operator+= (const BigInteger& rhs);
  const BigInteger operator+ (const BigInteger& rhs) const;

  BigInteger& operator-= (const BigInteger& rhs);
  const BigInteger operator- (const BigInteger& rhs) const;

  BigInteger& operator*= (const BigInteger& rhs);
  const BigInteger operator* (const BigInteger& rhs) const;

  virtual std::string getTypeName () const
  {
  	return "BigInteger";
  }

  bool operator<= (const BigInteger& rhs) const
	{
  	return (this->compare (rhs) <= 0);
	}

  bool operator< (const BigInteger& rhs) const
  {
  	return (this->compare (rhs) < 0);
  }

  bool operator>= (const BigInteger& rhs) const
	{
  	return (this->compare (rhs) >= 0);
	}

  bool operator> (const BigInteger& rhs) const
  {
  	return (this->compare (rhs) > 0);
  }

  bool operator== (const BigInteger& rhs) const
	{
  	return (this->compare (rhs) == 0);
	}

  bool isPositive () const
  {
  	return _positive;
  }

  bool switchSign ()
  {
  	_positive = !_positive;
  	return _positive;
  }

  void setPositive ()
  {
  	_positive = true;
  }

  void setNegative ()
  {
  	_positive = false;
  }

  virtual void insertIntoStream (std::ostream& os) const;
private:
	std::vector<BIGINT_BASETYPE> _bigNumber;
	bool _positive;

	unsigned int _baseTypeSize, _baseTypeHalfSize;
	BIGINT_BASETYPE _higherMask, _lowerMask;

	
	void setMasks ();


	int compare (const BigInteger& rhs) const;

	void cleanLeadingZeroes ();

	/**
	 * exclude eventual leading zero entries in the vector
	 */
	unsigned int getRealSize () const;

	/**
	 * helper function for the addition of BigIntegers, which handles the addition of value, which are half the size
	 * of BIGINT_BASETYPE
	 */
	BIGINT_BASETYPE addHalf (BIGINT_BASETYPE half1, BIGINT_BASETYPE half2, BIGINT_BASETYPE& result);

	BIGINT_BASETYPE getHigherPart (BIGINT_BASETYPE victim)
	{
		return ((victim & _higherMask) >> _baseTypeHalfSize);
	}

	BIGINT_BASETYPE getLowerPart (BIGINT_BASETYPE victim)
	{
		return (victim & _lowerMask);
	}
};

template <typename T>
void BigInteger::setFromNumber (T number)
{
	int absVal = std::abs (number);
	_bigNumber.clear ();
	int typeSize = sizeof (T) << 3;
	BIGINT_BASETYPE tempPackage;
	int packages = (typeSize + _baseTypeSize - 1) / _baseTypeSize;
	BIGINT_BASETYPE packageMask = -1;
	for (int i = 0; i < packages; i++)
	{
		tempPackage = absVal & packageMask;
		if (tempPackage > 0)
		{
			_bigNumber.push_back (tempPackage);
			absVal = absVal >> _baseTypeSize;
		}
	}
	_positive = (Utilities::sgn (number) >= 0);
}


}


