#include <iostream>
#include <cmath>
#include "BigInteger.h"
#include "Optional.h"
#include "RangeStream.h"
#include "InfiniteStream.h"

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
	RangeStream<int> range(1,10,1);
	std::function<float(int)> multABit = [](int a) -> float{return 1.5 * a;};
	range.filter([](int a){return (a % 2) == 0;}).map<float>([](int a) -> float{return sqrt(a);}).limit(3).forEach([](float t){std::cout << t << std::endl;});
	range.reset();
	InfiniteStream<int> evenNumbers([](int& seed){int temp = seed; seed += 2; return temp;}, 2);
	evenNumbers.limit(10).map<int>([](int a){return a*a;}).limit(5).forEach([](int a) {std::cout << a << std::endl;});
}
