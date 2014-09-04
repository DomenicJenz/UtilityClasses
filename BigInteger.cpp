/*
 * BigInteger.cpp
 *
 *  Created on: Apr 2, 2012
 *      Author: nucsoft
 */

#include "BigInteger.h"
#include "HelperFunctions.h"
#include <algorithm>
#include <ostream>

namespace Utilities
{

BigInteger::BigInteger ()
{
	setMasks ();
	_bigNumber.push_back (0);
	_positive = true;
}

BigInteger::BigInteger (int number)
{
	setMasks ();
	setFromNumber (number);
}

BigInteger::BigInteger (long number)
{
	setMasks ();
	setFromNumber (number);
}

BigInteger::BigInteger (const BigInteger& copy)
{
	setMasks ();
	_positive = copy._positive;
	_bigNumber.clear ();
	for (int unsigned i = 0; i < _bigNumber.size (); i++)
	{
		_bigNumber.push_back (copy._bigNumber[i]);
	}
}

BigInteger::~BigInteger ()
{
	// TODO Auto-generated destructor stub
}

BIGINT_BASETYPE BigInteger::addHalf (BIGINT_BASETYPE half1, BIGINT_BASETYPE half2, BIGINT_BASETYPE& carry)
{
	BIGINT_BASETYPE result = half1 + half2 + carry;
	carry = getHigherPart (result);
	result = getLowerPart (result);
	return result;
}

BigInteger& BigInteger::operator+= (const BigInteger& rhs)
{
	if (this->isPositive () && !rhs.isPositive ())
	{
		BigInteger tempRhs (rhs);
		tempRhs.setPositive ();
		*this -= tempRhs;
		return *this;
	}
	else if (!this->isPositive () && rhs.isPositive ())
	{
		this->setPositive ();
		*this = rhs - (*this);
		return *this;
	}
	unsigned int mySize = _bigNumber.size ();
	unsigned int otherSize = rhs._bigNumber.size ();
	//int min = DJBasicFunctions::min (mySize, otherSize);
	int maxSize = std::max (mySize, otherSize);

	if (maxSize > mySize)
	{
		// the result will be larger than me, so fill it up with zeroes before the addition process begins
		for (int i = mySize; i < maxSize; i++)
		{
			_bigNumber.push_back (0);
		}
	}

	BIGINT_BASETYPE carry = 0;
	for (int i = 0; i < maxSize; i++)
	{
		BIGINT_BASETYPE currentPartThis = _bigNumber[i];
		BIGINT_BASETYPE currentPartRhs = rhs._bigNumber[i];
		//BIGINT_BASETYPE lowerRes, higherRes;
		_bigNumber[i] = currentPartThis + currentPartRhs + carry;
		// overflow occured
		if ((_bigNumber[i] < currentPartThis) || ((_bigNumber[i] == currentPartThis) && (currentPartRhs > 0)))
		{
			carry = 1;
		}
		else
		{
			carry = 0;
		}
		//lowerRes = addHalf (getLowerPart (currentPartThis), getLowerPart (currentPartRhs), carry);
		//higherRes = addHalf (getHigherPart (currentPartThis), getHigherPart (currentPartRhs), carry);
		//_bigNumber[i] = (higherRes << _baseTypeHalfSize) + lowerRes;
	}

	if (carry > 0)
	{
		_bigNumber.push_back (carry);
	}

	return *this;
}

const BigInteger BigInteger::operator+ (const BigInteger& rhs) const
{
	BigInteger tempInt (*this);
	tempInt += rhs;
	return tempInt;
}

BigInteger& BigInteger::operator-= (const BigInteger& rhs)
{
	int comparison = this->compare (rhs);
	const std::vector<BIGINT_BASETYPE>* biggerSide;
	const std::vector<BIGINT_BASETYPE>* smallerSide;
	bool resultIsPositive;

	// we want to subtract the smaller number from the bigger one
	if (comparison >= 0)
	{
		resultIsPositive = true;
		biggerSide = &this->_bigNumber;
		smallerSide = &rhs._bigNumber;
	}
	else
	{
		resultIsPositive = false;
		biggerSide = &rhs._bigNumber;
		smallerSide = &this->_bigNumber;
	}

	// a - (-b) = a + b
	if (this->isPositive () && !rhs.isPositive ())
	{
		BigInteger tempRhs (rhs);
		tempRhs.setPositive ();
		*this += tempRhs;
		return *this;
	}
	// (-a) - b = -(a+b), this is already negative, as well as the result
	else if (!this->isPositive () && rhs.isPositive ())
	{
		*this += rhs;
		return *this;
	}
	// (-a) - (-b) = -a + b = b - a
	else if (!this->isPositive () && !rhs.isPositive ())
	{
		resultIsPositive = !resultIsPositive; // if a-b would have been positive, so b-a is negative
	}

	int carry = 0;
	int smallerSize = smallerSide->size ();
	int biggerSize = biggerSide->size ();
	BIGINT_BASETYPE partResult;
	BIGINT_BASETYPE currentBigger, currentSmaller;
	for (int i = 0; i < smallerSize; i++)
	{
		currentBigger = biggerSide->at (i);
		currentSmaller = smallerSide->at (i);
		partResult = currentBigger - currentSmaller - carry;
		// utilizing the effect of an underflow, where -1 = 255, a negative number will be "bigger" than the one from which was
		// subtracted
		if ((partResult > currentBigger) || ((partResult == currentBigger) && (currentSmaller > 0)))
		{
			carry = 1;
		}
		else
		{
			carry = 0;
		}
		_bigNumber[i] = partResult;
	}
	for (int i = smallerSize; i < biggerSize; i++)
	{
		partResult = biggerSide->at (i) - carry;
		if (partResult > biggerSide->at (i))
		{
			carry = 1;
		}
		else
		{
			carry = 0;
		}
		_bigNumber[i] = partResult;
	}
	cleanLeadingZeroes ();
	_positive = resultIsPositive;

	return *this;
}

const BigInteger BigInteger::operator- (const BigInteger& rhs) const
{
	BigInteger tempInt (*this);
	tempInt -= rhs;
	return tempInt;
}

BigInteger& BigInteger::operator*= (const BigInteger& rhs)
{
	return *this;
}

const BigInteger BigInteger::operator* (const BigInteger& rhs) const
{
	BigInteger tempInt (*this);
	tempInt *= rhs;
	return tempInt;
}

int BigInteger::compare (const BigInteger& rhs) const
{

	int mySize = getRealSize ();
	int otherSize = rhs.getRealSize ();
	// that handles the case 0 == 0. Both should be positive, but you never know ...
	if ((mySize == 0) && (otherSize == 0))
	{
		return 0;
	}
	if (this->isPositive () && !rhs.isPositive ())
	{
		return 1;
	}
	else if (!this->isPositive () && rhs.isPositive ())
	{
		return -1;
	}
	// now we can be sure that both numbers have the same sign, the modificator now handles the cases that both
	// are positive (1) or both are negative (-1)
	int modificator = this->isPositive () ? 1 : -1;

	if (mySize < otherSize)
	{
		return -1 * modificator;
	}
	else if (mySize > otherSize)
	{
		return 1 * modificator;
	}
	else
	{
		for (int i = mySize - 1; i >= 0; i++)
		{
			if (_bigNumber[i] < rhs._bigNumber[i])
			{
				return -1 * modificator;
			}
			else if (_bigNumber[i] > rhs._bigNumber[i])
			{
				return 1 * modificator;
			}
		}
		return 0;
	}
}

void BigInteger::setFromString (std::string numberString)
{
	//int stringSize = numberString.size ();

}

void BigInteger::cleanLeadingZeroes ()
{

	for (int i = _bigNumber.size () - 1; i >= 0; i--)
	{
		if (_bigNumber[i] == 0)
		{
			_bigNumber.erase (_bigNumber.begin () + i);
		}
	}
}

unsigned int BigInteger::getRealSize () const
{
	int vectorSize = _bigNumber.size ();
	for (int i = vectorSize - 1; i >= 0; i--)
	{
		if (_bigNumber[i] != 0)
		{
			return i + 1;
		}
	}
	return 0;
}

void BigInteger::insertIntoStream (std::ostream& os) const
{
	os << "BigInteger ( ";
	if (!_positive)
	{
		os << "-";
	}
	for (int i = _bigNumber.size() - 1; i >= 0; i--)
	{
		os << std::hex << (unsigned int) _bigNumber[i] << " ";
	}
	os << std::dec << ")";
}

void BigInteger::setMasks ()
{
	_baseTypeSize = sizeof (BIGINT_BASETYPE) << 3;
	_baseTypeHalfSize = _baseTypeSize >> 1;
	_lowerMask = ((BIGINT_BASETYPE) 1 << _baseTypeHalfSize) - 1;
	_higherMask = _lowerMask << _baseTypeHalfSize;
}

}

