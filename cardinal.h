#ifndef CARDINAL_NUMBER
//#include <bitset>
#include <vector>

#define BIT_SIZE			128
#define SIGN				0
#define INTEGER_FIRST		1
#define INTEGER_LAST		87
#define FRACTIONAL_FIRST	88
#define FRACTIONAL_LAST		128

struct cardinal
{
	//std::bitset<BIT_SIZE> data;
	std::vector<bool> data;

	cardinal(const cardinal& value) noexcept
		: data(value.data)
	{}

	cardinal(const long long& value) noexcept
	{

	}
};

//struct cardinal
//{
//	long long int_section_1, int_section_2, int_section_3; // just as they were concatenated, first bit is empty (int_section_2, int_section_3), first bit is sign (int_section_1)
//	double fractional_section; // < 1
//
//	cardinal()
//	{
//		int_section_1		= 0;
//		int_section_2		= 0;
//		int_section_3		= 0;
//		fractional_section	= 0.0;
//	}
//	
//	cardinal(const cardinal& value)
//	{
//		int_section_1		= value.int_section_1;
//		int_section_2		= value.int_section_2;
//		int_section_3		= value.int_section_3;
//		fractional_section	= value.fractional_section;
//	}
//
//	cardinal(const long long& value)
//	{
//		int_section_1		= value & (1ll << 63);
//		int_section_2		= 0;
//		int_section_3		= value & ~(1ll << 63);
//		fractional_section	= 0.0;
//	}
//	
//	cardinal(const unsigned long long& value)
//	{
//		int_section_1		= value & (1ll << 63);
//		int_section_2		= (value & 1ll << 63) >> 63;
//		int_section_3		= value & ~(1ll << 63);
//		fractional_section	= 0.0;
//	}
//
//	/*cardinal(char value[])
//	{
//
//	}*/
//
//	/*cardinal(double value)
//	{
//
//	}*/
//
//	cardinal operator + (const cardinal& other) const // negatives aren't supported
//	{
//		cardinal result(*this);
//		result.fractional_section += other.fractional_section;
//		if (result.fractional_section >= 1.0)
//		{
//			result.int_section_3 ++;
//			result.fractional_section -= 1;
//		}
//		result.int_section_3 += other.int_section_3;
//		if (result.int_section_3 < 0)
//		{
//			result.int_section_2 ++;
//			result.int_section_3 &= ~(1ll << 63);
//		}
//		result.int_section_2 += other.int_section_2;
//		if (result.int_section_2 < 0)
//		{
//			result.int_section_1 ++;
//			result.int_section_2 &= ~(1ll << 63);
//		}
//		result.int_section_1 += other.int_section_1;
//		return result;
//	}
//};

#define CARDINAL_NUMBER

#endif // CARDINAL_NUMBER