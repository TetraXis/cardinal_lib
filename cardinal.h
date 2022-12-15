#ifndef CARDINAL_NUMBER
#include <string>

/*

		cardinal structure:
		
		256 bits
		1 bit - sign
		next 191 bits - integer part 
		next 64 bits - fractional part
		
		bits represented using 4 unsigned long longs:
		left, middle, right, fractional

*/

#define BIT_SIZE			256
#define SIGN				0
#define INTEGER_LEFT		1
#define INTEGER_RIGHT		191
#define INTEGER_SIZE		191
#define FRACTIONAL_LEFT		192
#define FRACTIONAL_RIGHT	255
#define FRACTIONAL_SIZE		64
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

struct cardinal
{
	struct
	{
		unsigned long long left = 0, middle = 0, right = 0, fractional = 0;

		bool operator [](const unsigned int& position) const
		{
			if (position < 64)
			{
				return left & (1ull << (63-position));
			}
			if (position < 128)
			{
				return middle & (1ull << (127 - position));
			}
			if (position < 192)
			{
				return right & (1ull << (191 - position));
			}
			return fractional & (1ull << (255 - position));
		}

		void set_at(const unsigned int& position, const bool& value)
		{
			if (value)
			{
				if (position < 64)
				{
					left |= 1ull << (63 - position);
					return;
				}
				if (position < 128)
				{
					middle |= 1ull << (127 - position);
					return;
				}
				if (position < 192)
				{
					right |= 1ull << (191 - position);
					return;
				}
				fractional |= 1ull << (255 - position);
				return;
			}
			if (position < 64)
			{
				left &= ~(1ull << (63 - position));
				return;
			}
			if (position < 128)
			{
				middle &= ~(1ull << (127 - position));
				return;
			}
			if (position < 192)
			{
				right &= ~(1ull << (191 - position));
				return;
			}
			fractional &= ~(1ull << (255 - position));
			return;
			/*if (position < 64)
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
			right &= ~(1ull << ((127 - position)));*/
		}

		void flip()
		{
			left = ~left;
			middle = ~middle;
			right = ~right;
			fractional = ~fractional;
		}
	} bits;

	cardinal()
	{}

	cardinal(const cardinal& value)
		: bits(value.bits)
	{}

	cardinal(long long value) noexcept
	{
		bool flip = value < 0;
		if (flip)
		{
			value = -value;
		}
		bits.right = value;
		if (flip)
		{
			invert();
		}
	}

	cardinal(int value) noexcept
	{
		bool flip = value < 0;
		if (flip)
		{
			value = -value;
		}
		bits.right = value;
		if (flip)
		{
			invert();
		}
	}
	
	cardinal(unsigned long long value) noexcept
	{
		bits.right = value;
	}

	cardinal(unsigned int value) noexcept
	{
		bits.right = value;
	}

	cardinal(double value) noexcept
	{
		bool flip = value < 0;
		if (flip)
		{
			value = -value;
		}
		unsigned long long double_bits = *(unsigned long long*)(&value);
		int exponent = ((double_bits & (0b11111111111ull << 52)) >> 52) - 1023;
		unsigned long long mantissa = (double_bits & ((1ull << 52) - 1)) + (1ull << 52);
		for (int i = INTEGER_RIGHT + 52 - exponent; mantissa != 0 && i >= 0; i--)
		{
			if (i < BIT_SIZE)
			{
				bits.set_at(i, mantissa & 1);
			}
			mantissa >>= 1;
		}
		if (flip)
		{
			invert();
		}
	}

	cardinal(const string& value)
	{
		if (value[0] == '0' && value[1] == 'b') /* binary literal */
		{
			for (int i = value.size() - 1, j = BIT_SIZE - 1; i > 1; --i)
			{
				if (value[i] != '0' && value[i] != '1')
				{
					continue;
				}
				bits.set_at(j, value[i] != '0');
				j--;
			}
			return;
		}
	}

	void operator <<= (const int& shift)
	{
		if (shift < 0)
		{
			*this >>= (-shift);
			return;
		}
		for (int i = 0; i < BIT_SIZE; i++)
		{
			if (i + shift < BIT_SIZE)
			{
				bits.set_at(i, bits[i + shift]);
			}
			else
			{
				bits.set_at(i, false);
			}
		}
		/*for (int i = BIT_SIZE - shift; i < BIT_SIZE; i++)
		{
			bits.set_at(i, false);
		}*/
	}

	void operator >>= (const int& shift)
	{
		if (shift < 0)
		{
			*this <<= (-shift);
			return;
		}
		for (int i = BIT_SIZE - 1; i >= 0; i--)
		{
			if (i - shift >= 0)
			{
				bits.set_at(i, bits[i - shift]);
			}
			else
			{
				bits.set_at(i, bits[SIGN]);
			}
		}
		/*for (int i = BIT_SIZE - shift; i < BIT_SIZE; i++)
		{
			bits.set_at(i, false);
		}*/
	}

	/* need to do them expicetly */

	cardinal operator << (const int& shift) const
	{
		cardinal result(*this);
		result <<= shift;
		return result;
	}

	cardinal operator >> (const int& shift) const
	{
		cardinal result(*this);
		result >>= shift;
		return result;
	}

	cardinal operator - () const
	{
		return inverted();
	}

	cardinal operator + (const cardinal& other) const
	{
		cardinal result(*this);
		bool fractional_overflow, right_overflow, middle_overflow;

		fractional_overflow = ULL_MAX_VALUE - result.bits.fractional < other.bits.fractional;
		result.bits.fractional += other.bits.fractional;
																									/* \/ \/ \/ говно, возможно не работает */
		right_overflow = ULL_MAX_VALUE - result.bits.right - fractional_overflow < other.bits.right || fractional_overflow && ULL_MAX_VALUE - result.bits.right - fractional_overflow == ULL_MAX_VALUE;
		result.bits.right += other.bits.right + fractional_overflow;

		middle_overflow = ULL_MAX_VALUE - result.bits.middle - right_overflow < other.bits.middle || right_overflow && ULL_MAX_VALUE - result.bits.middle - right_overflow == ULL_MAX_VALUE;
		result.bits.middle += other.bits.middle + right_overflow;

		result.bits.left += other.bits.left + middle_overflow;

		return result;

		//cardinal result;
		//bool remaining = false;
		//for (int i = BIT_SIZE - 1; i >= 0; i--)
		//{
		//	//result.bits[i] = bits[i] ^ other.bits[i] ^ remaining;
		//	result.bits.set_at(i, bits[i] ^ other.bits[i] ^ remaining);
		//	remaining = bits[i] & other.bits[i] | bits[i] & remaining | remaining & other.bits[i];
		//}
		//return result;
	}

	void operator += (const cardinal & other)
	{
		bool fractional_overflow, right_overflow, middle_overflow;

		fractional_overflow = ULL_MAX_VALUE - bits.fractional < other.bits.fractional;
		bits.fractional += other.bits.fractional;
		/* \/ \/ \/ говно, возможно не работает */
		right_overflow = ULL_MAX_VALUE - bits.right - fractional_overflow < other.bits.right || fractional_overflow && ULL_MAX_VALUE - bits.right - fractional_overflow == ULL_MAX_VALUE;
		bits.right += other.bits.right + fractional_overflow;

		middle_overflow = ULL_MAX_VALUE - bits.middle - right_overflow < other.bits.middle || right_overflow && ULL_MAX_VALUE - bits.middle - right_overflow == ULL_MAX_VALUE;
		bits.middle += other.bits.middle + right_overflow;

		bits.left += other.bits.left + middle_overflow;
	}

	cardinal operator - (const cardinal& other) const
	{
		return *this + other.inverted();
	}

	void operator -= (const cardinal& other)
	{
		*this += other.inverted();
	}

	cardinal operator * (cardinal other) const
	{
		cardinal result;
		int shift = -64;
		while (other.bits.fractional != 0)
		{
			if (other.bits.fractional & 1)
			{
				result += *this << shift;
			}
			other.bits.fractional >>= 1;
			++shift;
		}
		shift = 0;
		while (other.bits.right != 0)
		{
			if (other.bits.right & 1)
			{
				result += *this << shift;
			}
			other.bits.right >>= 1;
			++shift;
		}
		shift = 64;
		while (other.bits.middle != 0)
		{
			if (other.bits.middle & 1)
			{
				result += *this << shift;
			}
			other.bits.middle >>= 1;
			++shift;
		}
		shift = 128;
		while (other.bits.left != 0)
		{
			if (other.bits.left & 1)
			{
				result += *this << shift;
			}
			other.bits.left >>= 1;
			++shift;
		}
		return result;
	}

	void operator *= (cardinal other)
	{
		cardinal result;
		int shift = -64;
		while (other.bits.fractional != 0)
		{
			if (other.bits.fractional & 1)
			{
				result += *this << shift;
			}
			other.bits.fractional >>= 1;
			++shift;
		}
		shift = 0;
		while (other.bits.right != 0)
		{
			if (other.bits.right & 1)
			{
				result += *this << shift;
			}
			other.bits.right >>= 1;
			++shift;
		}
		shift = 64;
		while (other.bits.middle != 0)
		{
			if (other.bits.middle & 1)
			{
				result += *this << shift;
			}
			other.bits.middle >>= 1;
			++shift;
		}
		shift = 128;
		while (other.bits.left != 0)
		{
			if (other.bits.left & 1)
			{
				result += *this << shift;
			}
			other.bits.left >>= 1;
			++shift;
		}
		*this = result;
	}

	/* Mb incorrect when overflowing */

	cardinal operator ++ (int)
	{
		cardinal temp(*this);
		int i;
		for (i = BIT_SIZE - FRACTIONAL_SIZE - 1; bits[i] && i >= 0; i--)
		{
			//bits[i] = false;
			bits.set_at(i, false);
		}
		if (i > 0)
		{
			//bits[i] = true;
			bits.set_at(i, true);
		}
		return temp;
	}
	
	cardinal operator ++ ()
	{
		int i;
		for (i = BIT_SIZE - FRACTIONAL_SIZE - 1; bits[i] && i >= 0; i--)
		{
			//bits[i] = false;
			bits.set_at(i, false);
		}
		if (i > 0)
		{
			//bits[i] = true;
			bits.set_at(i, true);
		}
		return *this;
	}

	cardinal operator -- (int)
	{
		cardinal temp(*this);
		int i;
		for (i = BIT_SIZE - FRACTIONAL_SIZE - 1; !bits[i] && i >= 0; i--)
		{
			//bits[i] = true;
			bits.set_at(i, true);
		}
		if (i > 0)
		{
			//bits[i] = false;
			bits.set_at(i, false);
		}
		return temp;
	}

	cardinal operator -- ()
	{
		int i;
		for (i = BIT_SIZE - FRACTIONAL_SIZE - 1; !bits[i] && i >= 0; i--)
		{
			//bits[i] = true;
			bits.set_at(i, true);
		}
		if (i > 0)
		{
			//bits[i] = false;
			bits.set_at(i, false);
		}
		return *this;
	}

	explicit operator int() const noexcept
	{
		int result = int(bits.right);
		if (bits[SIGN])
		{
			result |= 1 << 31;
		}
		return result;
	}

	explicit operator unsigned int() const noexcept
	{
		return unsigned int (bits.right);
	}

	explicit operator long long() const noexcept
	{
		long long result = bits.right;
		if (bits[SIGN])
		{
			result |= 1ull << 63;
		}
		return result;
	}

	explicit operator unsigned long long() const noexcept
	{
		return bits.right;
	}

	operator std::string() const noexcept
	{
		std::string result = (bits[SIGN] ? "1 " : "0 ");
		result += " ";
		for (int i = INTEGER_LEFT; i <= INTEGER_RIGHT; i++)
		{
			result += bits[i] + '0';
		}
		result += " ";
		for (int i = FRACTIONAL_LEFT; i <= FRACTIONAL_RIGHT; i++)
		{
			result += bits[i] + '0';
		}
		return result;
	}

	std::string to_binary(const bool& add_spaces = false) const noexcept
	{
		std::string result = (bits[SIGN] ? "1" : "0");
		if (add_spaces)
		{
			result += " ";
		}
		for (int i = INTEGER_LEFT; i <= INTEGER_RIGHT; i++)
		{
			result += bits[i] + '0';
		}
		if (add_spaces)
		{
			result += " ";
		}
		for (int i = FRACTIONAL_LEFT; i <= FRACTIONAL_RIGHT; i++)
		{
			result += bits[i] + '0';
		}
		return result;
	}

	void increment() noexcept /* add smallest amount */
	{
		int i;
		for (i = BIT_SIZE - 1; bits[i] && i >= 0; i--)
		{
			//bits[i] = false;
			bits.set_at(i, false);
		}
		if (i > 0)
		{
			//bits[i] = true;
			bits.set_at(i, true);
		}
	}
	
	void decrement() noexcept /* substruct smallest amount */
	{
		int i;
		for (i = BIT_SIZE - 1; !bits[i] && i >= 0; i--)
		{
			//bits[i] = true;
			bits.set_at(i, true);
		}
		if (i > 0)
		{
			//bits[i] = false;
			bits.set_at(i, false);
		}
	}

	void invert() /* changes sign */
	{
		if (bits[SIGN])
		{
			decrement();
			bits.flip();
		}
		else
		{
			bits.flip();
			increment();
		}
	}

	cardinal inverted() const
	{
		cardinal result(*this);
		result.invert();
		return result;
	}

	bool get_bit(const unsigned int& position) const
	{
		return bool(bits[position]);
	}

	void set_bit(const unsigned int& position, const bool& value = false)
	{
		//bits[position] = value;
		bits.set_at(position, value);
	}

	//inline void flip_bit(const unsigned int& position)
	//{
	//	//bits[position] = !bits[position];
	//	bits.set_at(position, value);
	//}

	//void flip_all() noexcept
	//{
	//	bits.flip();
	//}

	//void flip_sign_bit() noexcept
	//{
	//	bits[SIGN].flip();
	//}

	//constexpr void flip_integer_bits() noexcept
	//{
	//	for (unsigned int i = INTEGER_LEFT; i <= INTEGER_RIGHT; i++)
	//	{
	//		bits[i] = !bits[i];
	//	}
	//}

	//constexpr void flip_fractional_bits() noexcept
	//{
	//	for (unsigned int i = FRACTIONAL_LEFT; i <= FRACTIONAL_RIGHT; i++)
	//	{
	//		bits[i] = !bits[i];
	//	}
	//}

	/*std::bitset::reference& operator [] (const unsigned int& position)
	{
		return bits[position];
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