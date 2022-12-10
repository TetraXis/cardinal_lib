#ifndef CARDINAL_NUMBER
#include <bitset>
//#include <vector>

#define BIT_SIZE			128
#define SIGN				0
#define INTEGER_LEFT		1
#define INTEGER_RIGHT		87
#define INTEGER_SIZE		87
#define FRACTIONAL_LEFT		88
#define FRACTIONAL_RIGHT	127
#define FRACTIONAL_SIZE		40
#define ULL_MAX_VALUE		18446744073709551615

template <typename T>
std::string as_binary(T value)
{
	const unsigned int size = sizeof(T) * 8;
	unsigned long long new_value = *(unsigned long long*)(&value);
	string result = "";
	for (unsigned int i = 0; i < size; i++)
	{
		result += (new_value & 1) + '0';
		new_value >>= 1;
	}
	for (unsigned int i = 0; i < size/2; i++)
	{
		std::swap(result[i], result[size - i - 1]);
	}
	return result;
}

/*
9.0949477017729282379150390625
90949477017729282379150390625 e-28

90949477017729282379150390625

38146975525337
2384185791015625

*/

struct cardinal
{
	//std::bitset<BIT_SIZE> data;
private:
	//std::vector<bool> data = vector<bool>(BIT_SIZE, false);
	//std::bitset<BIT_SIZE> data;
	struct
	{
		long long left = 0;
		unsigned long long right = 0;

		bool operator [](const unsigned int& position) const
		{
			if (position < 64)
			{
				return left & (1ull << (63-position));
			}
			return right & (1ull << (127-position));
		}

		void set_at(const unsigned int& position, const bool& value)
		{
			if (position < 64)
			{
				if (value)
				{
					left |= 1ull << (63 - position);
					return;
				}
				left &= ~(1ull << (63 - position));
				return;
			}
			if (value)
			{
				right |= 1ull << ((127 - position));
				return;
			}
			right &= ~(1ull << ((127 - position)));
		}

		void flip()
		{
			left = ~left;
			right = ~right;
		}
	} data;

public:
	cardinal()
	{}

	cardinal(const cardinal& value)
		: data(value.data)
	{}

	cardinal(long long value) noexcept
	{
		for (unsigned int i = INTEGER_RIGHT; value != 0 && i > INTEGER_RIGHT - 63; i--)
		{
			//data[i] = value & 1;
			data.set_at(i, value & 1);
			value >>= 1;
		}
		if (value < 0)
		{
			invert();
		}
	}
	
	cardinal(unsigned long long value) noexcept
	{
		for (unsigned int i = INTEGER_RIGHT; value != 0 && i > INTEGER_RIGHT - 64; i--)
		{
			//data[i] = value & 1;
			data.set_at(i, value & 1);
			value >>= 1;
		}
		//data[INTEGER_RIGHT - 64] = value & 1;
		data.set_at(INTEGER_RIGHT - 64, value & 1);
	}

	cardinal(double value) noexcept
	{
		bool flip = value < 0;
		if (flip)
		{
			value = -value;
		}
		unsigned long long bits = *(unsigned long long*)(&value);
		int exponent = ((bits & (0b11111111111ull << 52)) >> 52) - 1023;
		unsigned long long mantissa = (bits & ((1ull << 52) - 1)) + (1ull << 52);
		for (int i = INTEGER_RIGHT + 52 - exponent; mantissa != 0 && i >= 0; i--)
		{
			if (i < BIT_SIZE)
			{
				//data[i] = mantissa & 1;
				data.set_at(i, mantissa & 1);
			}
			mantissa >>= 1;
		}
		if (flip)
		{
			invert();
		}
	}

	//void operator <<= ()

	cardinal operator + (const cardinal& other) const
	{
		cardinal result(*this);
		if (ULL_MAX_VALUE - data.right < other.data.right)
		{
			result.data.left++;
		}
		result.data.right += other.data.right;
		result.data.left += other.data.left;
		return result;

		//cardinal result;
		//bool remaining = false;
		//for (int i = BIT_SIZE - 1; i >= 0; i--)
		//{
		//	//result.data[i] = data[i] ^ other.data[i] ^ remaining;
		//	result.data.set_at(i, data[i] ^ other.data[i] ^ remaining);
		//	remaining = data[i] & other.data[i] | data[i] & remaining | remaining & other.data[i];
		//}
		//return result;
	}

	/* Mb incorrect when overflowing */

	cardinal operator ++ (int)
	{
		cardinal temp(*this);
		int i;
		for (i = BIT_SIZE - FRACTIONAL_SIZE - 1; data[i] && i >= 0; i--)
		{
			//data[i] = false;
			data.set_at(i, false);
		}
		if (i > 0)
		{
			//data[i] = true;
			data.set_at(i, true);
		}
		return temp;
	}
	
	cardinal operator ++ ()
	{
		int i;
		for (i = BIT_SIZE - FRACTIONAL_SIZE - 1; data[i] && i >= 0; i--)
		{
			//data[i] = false;
			data.set_at(i, false);
		}
		if (i > 0)
		{
			//data[i] = true;
			data.set_at(i, true);
		}
		return *this;
	}

	cardinal operator -- (int)
	{
		cardinal temp(*this);
		int i;
		for (i = BIT_SIZE - FRACTIONAL_SIZE - 1; !data[i] && i >= 0; i--)
		{
			//data[i] = true;
			data.set_at(i, true);
		}
		if (i > 0)
		{
			//data[i] = false;
			data.set_at(i, false);
		}
		return temp;
	}

	cardinal operator -- ()
	{
		int i;
		for (i = BIT_SIZE - FRACTIONAL_SIZE - 1; !data[i] && i >= 0; i--)
		{
			//data[i] = true;
			data.set_at(i, true);
		}
		if (i > 0)
		{
			//data[i] = false;
			data.set_at(i, false);
		}
		return *this;
	}

	operator std::string() const noexcept
	{
		std::string result = (data[SIGN] ? "1 " : "0 ");
		result += " ";
		for (int i = INTEGER_LEFT; i <= INTEGER_RIGHT; i++)
		{
			result += data[i] + '0';
		}
		result += " ";
		for (int i = FRACTIONAL_LEFT; i <= FRACTIONAL_RIGHT; i++)
		{
			result += data[i] + '0';
		}
		return result;
	}

	std::string to_binary(const bool& add_spaces = false) const noexcept
	{
		std::string result = (data[SIGN] ? "1" : "0");
		if (add_spaces)
		{
			result += " ";
		}
		for (int i = INTEGER_LEFT; i <= INTEGER_RIGHT; i++)
		{
			result += data[i] + '0';
		}
		if (add_spaces)
		{
			result += " ";
		}
		for (int i = FRACTIONAL_LEFT; i <= FRACTIONAL_RIGHT; i++)
		{
			result += data[i] + '0';
		}
		return result;
	}

	void increment() noexcept /* add smallest amount */
	{
		int i;
		for (i = BIT_SIZE - 1; data[i] && i >= 0; i--)
		{
			//data[i] = false;
			data.set_at(i, false);
		}
		if (i > 0)
		{
			//data[i] = true;
			data.set_at(i, true);
		}
	}
	
	void decrement() noexcept /* substruct smallest amount */
	{
		int i;
		for (i = BIT_SIZE - 1; !data[i] && i >= 0; i--)
		{
			//data[i] = true;
			data.set_at(i, true);
		}
		if (i > 0)
		{
			//data[i] = false;
			data.set_at(i, false);
		}
	}

	void invert() /* changes sign */
	{
		if (data[SIGN])
		{
			decrement();
			data.flip();
		}
		else
		{
			data.flip();
			increment();
		}
	}

	bool get_bit(const unsigned int& position) const
	{
		return bool(data[position]);
	}

	void set_bit(const unsigned int& position, const bool& value = false)
	{
		//data[position] = value;
		data.set_at(position, value);
	}

	//inline void flip_bit(const unsigned int& position)
	//{
	//	//data[position] = !data[position];
	//	data.set_at(position, value);
	//}

	//void flip_all() noexcept
	//{
	//	data.flip();
	//}

	//void flip_sign_bit() noexcept
	//{
	//	data[SIGN].flip();
	//}

	//constexpr void flip_integer_bits() noexcept
	//{
	//	for (unsigned int i = INTEGER_LEFT; i <= INTEGER_RIGHT; i++)
	//	{
	//		data[i] = !data[i];
	//	}
	//}

	//constexpr void flip_fractional_bits() noexcept
	//{
	//	for (unsigned int i = FRACTIONAL_LEFT; i <= FRACTIONAL_RIGHT; i++)
	//	{
	//		data[i] = !data[i];
	//	}
	//}

	/*std::bitset::reference& operator [] (const unsigned int& position)
	{
		return data[position];
	}*/
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