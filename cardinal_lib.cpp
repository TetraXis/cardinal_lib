﻿/*

	This file is just for testing

*/

#include <iostream>
#include <chrono>

#define AMOUNT_OF_TESTS 20
#define TEST_DURATION 1000000

using namespace std;
using namespace std::chrono;

#include "cardinal.h"

int main()
{
	cardinal a = 1.0, b = 13.0;
	double sum = 0;
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
	cout << "avg:\t" << sum / AMOUNT_OF_TESTS << " ms" << endl;
	cout << "\n\nreturn\n";
	return 0;
}
