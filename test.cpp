#include <iostream>
#include "BigInteger.h"
#include "Optional.h"

using namespace Utilities;

void bigIntegerTest ()
{
	BigInteger big1(12364);
	big1.setFromString("1253");
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
	BigInteger big5 = 12345678901234567890_bigInt;
	std::cout << big5 << std::endl;
	std::cout << big5 + 98765432109876543210_bigInt << std::endl;
}

int main (int argc, char** argv)
{
	Optional<int> opt1 {33};
	auto opt2 = optionalValue (3.4);
	auto opt3 = optionalNothing<std::string>();
	std::cout << opt1.getValue(0) << std::endl;
	std::cout << opt2.getValue ()<< std::endl;
	std::cout << opt3.getValue ("blubb") << std::endl;
}
