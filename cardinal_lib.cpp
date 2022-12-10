/*

	This file is just for testing

*/

#include <iostream>
#include <chrono>
#include <Windows.h>

#define AMOUNT_OF_TESTS 20
#define TEST_DURATION 100000

using namespace std;
using namespace std::chrono;

#include "cardinal.h"

int main()
{
	cardinal a = 12, b = -1;
	cout << (a).to_binary(true) << '\n';
	cout << (b).to_binary(true) << '\n';
	cout << (a - b).to_binary(true) << '\n';
	/*double sum = 0;
	for (int j = 0; j < AMOUNT_OF_TESTS; j++)
	{
		uint64_t ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
		for (int i = 0; i < TEST_DURATION; i++)
		{
			a = a + b;
		}
		cout << j << ":\t" << duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count() - ms << " ms" << endl;
		sum += duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count() - ms;
	}
	cout << "avg:\t" << sum / AMOUNT_OF_TESTS << " ms" << endl;*/
	cout << "\n\nreturn\n";
	return 0;
}
