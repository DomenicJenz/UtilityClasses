#include <iostream>
#include "BigInteger.h"

using namespace Utilities;

int main (int argc, char** argv)
{
	BigInteger big1(1234);
	BigInteger big2(4321);
	BigInteger big3;
	std::cout << big1.asString() << " * " << big2<< std::endl;
	big3 = big2 * big1;
	std::cout << big3 << std::endl;
	big3 /= 10;
	std::cout << big3 << std::endl;
	BigInteger big4 = big3 / 10;
	std::cout << big4 << std::endl;
	big4 *= 10;
	std::cout << big4 << std::endl;
}
