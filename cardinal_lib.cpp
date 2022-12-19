/*

	This file is just for testing

*/

#include <iostream>
#include <chrono>
#include <Windows.h>

#define AMOUNT_OF_TESTS 30
#define TEST_DURATION 100000000

using namespace std;
using namespace std::chrono;

#include "cardinal.h"

#define TEST false

int main()
{
	cardinal a = -1, b = 1;

	cout << a.to_binary(true) << endl;
	a.increment();
	cout << a.to_binary(true) << endl;
	cout << (b+a).to_binary(true) << endl;

	//cardinal a("0b0"
	//	"111111111111111111111111111111111111111111111111111111111111111"
	//	"1111111111111111111111111111111111111111111111111111111111111111"
	//	"1111111111111111111111111111111111111111111111111111111111111111"
	//	"0000000000000000000000000000000000000000000000000000000000000000");

	if (TEST) // =============================================================================
	{
		double sum = 0;
		uint64_t ms1, ms2;
		for (int j = 0; j < AMOUNT_OF_TESTS; j++)
		{
			ms1 = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
			for (int i = 0; i < TEST_DURATION; i++)
			{

			}
			ms2 = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
			cout << j << '\t' << ms2 - ms1 << " ms" << endl;
			sum += ms2 - ms1;
		}
		cout << "avg:\t" << sum / AMOUNT_OF_TESTS << " ms" << endl;
	} // =====================================================================================

	cout << "\n\nreturn\n";
	return 0;
}
