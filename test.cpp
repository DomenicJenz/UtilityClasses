#include <iostream>
#include "BigInteger.h"
#include "Optional.h"
#include "RangeStream.h"

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
	RangeStream<int> range(1,10,3);
	range.forEach([](int t){std::cout << t << std::endl;});
	range.reset();
	auto adding = [](int a, int b){return a+b;};
	std::function<int(int,int)> addFunc = adding;
	std::cout << adding(1,2) << " " << addFunc(2,3) << std::endl;
	std::cout << range.foldLeft(addFunc, 0) << std::endl;
	range.reset();
	std::cout << range.foldLeft([](int a, int b) -> int {return a*b;}, 1) << std::endl;
	RangeStream<float> rangeF (1.0, 3.0, 1.0);
	std::cout << rangeF.foldLeft([](float a, float b) {return a/b;}, 100.0) << std::endl;
	rangeF.reset();
	std::cout << rangeF.foldRight([](float a, float b) {return a/b;}, 100.0) << std::endl;
}
