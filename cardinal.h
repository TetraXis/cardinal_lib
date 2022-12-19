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

		const bool operator [](const unsigned int& position) const noexcept
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

		void set_at(const unsigned int& position, const bool& value) noexcept
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
		}

		void flip() noexcept
		{
			left = ~left;
			middle = ~middle;
			right = ~right;
			fractional = ~fractional;
		}
	} bits;

	cardinal()
	{}

	cardinal(const cardinal& value) noexcept
		: bits(value.bits)
	{}

	cardinal(const long long& value) noexcept
	{
		if (value < 0)
		{
			bits.right = -value;
			invert();
			return;
		}
		bits.right = value;
	}

	cardinal(const int& value) noexcept
	{
		if (value < 0)
		{
			bits.right = -value;
			invert();
			return;
		}
		bits.right = value;
	}
	
	cardinal(const unsigned long long& value) noexcept
	{
		bits.right = value;
	}

	cardinal(const unsigned int& value) noexcept
	{
		bits.right = value;
	}

	cardinal(const double& value) noexcept
	{
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
		if (double_bits & 1ull<<63)
		{
			invert();
		}
	}

	cardinal(const string& value) noexcept
	{
		if (value[0] == '0' && value[1] == 'b') /* binary literal */
		{
			for (int i = value.size() - 1, j = BIT_SIZE - 1; i > 1; --i)
			{
				if (value[i] != '0' && value[i] != '1')
				{
					continue;
				}
				bits.set_at(j, value[i] == '1');
				j--;
			}
			return;
		}
	}

	void operator <<= (const int& shift) noexcept
	{
		if (shift < 0)
		{
			*this >>= (-shift);
			return;
		}
		for (int i = 0; i < BIT_SIZE; ++i)
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
	}

	void operator >>= (const int& shift) noexcept
	{
		if (shift < 0)
		{
			*this <<= (-shift);
			return;
		}
		bool sign = bits[SIGN];
		for (int i = BIT_SIZE - 1; i >= 0; --i)
		{
			if (i - shift >= 0)
			{
				bits.set_at(i, bits[i - shift]);
			}
			else
			{
				bits.set_at(i, sign);
			}
		}
	}

	const cardinal operator << (const int& shift) const noexcept
	{
		cardinal result(*this);
		result <<= shift;
		return result;
	}

	const cardinal operator >> (const int& shift) const noexcept
	{
		cardinal result(*this);
		result >>= shift;
		return result;
	}

	const cardinal operator - () const noexcept
	{
		return inverted();
	}

	const cardinal operator + (const cardinal& other) const noexcept
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
	}

	const cardinal operator + (const unsigned long long& other) const noexcept
	{
		cardinal result(*this);

		bool right_overflow, middle_overflow;

		right_overflow = ULL_MAX_VALUE - bits.right < other;
		result.bits.right += other;

		middle_overflow = right_overflow && ULL_MAX_VALUE - bits.middle - right_overflow == ULL_MAX_VALUE;
		result.bits.middle += right_overflow;

		result.bits.left += middle_overflow;

		return result;
	}

	void operator += (const cardinal& other) noexcept
	{
		bool fractional_overflow, right_overflow, middle_overflow;

		fractional_overflow = ULL_MAX_VALUE - bits.fractional < other.bits.fractional;
		bits.fractional += other.bits.fractional;

		right_overflow = ULL_MAX_VALUE - bits.right - fractional_overflow < other.bits.right || fractional_overflow && ULL_MAX_VALUE - bits.right - fractional_overflow == ULL_MAX_VALUE;
		bits.right += other.bits.right + fractional_overflow;

		middle_overflow = ULL_MAX_VALUE - bits.middle - right_overflow < other.bits.middle || right_overflow && ULL_MAX_VALUE - bits.middle - right_overflow == ULL_MAX_VALUE;
		bits.middle += other.bits.middle + right_overflow;

		bits.left += other.bits.left + middle_overflow;
	}

	void operator += (const unsigned long long & other) noexcept
	{
		bool right_overflow, middle_overflow;

		right_overflow = ULL_MAX_VALUE - bits.right < other;
		bits.right += other;

		middle_overflow = right_overflow && ULL_MAX_VALUE - bits.middle - right_overflow == ULL_MAX_VALUE;
		bits.middle += right_overflow;

		bits.left += middle_overflow;
	}

	const cardinal operator - (const cardinal& other) const noexcept
	{
		return *this + other.inverted();
	}

	void operator -= (const cardinal& other) noexcept
	{
		*this += other.inverted();
	}

	const cardinal operator * (cardinal other) const noexcept
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

	void operator *= (cardinal other) noexcept
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

	cardinal operator ++ (int) noexcept
	{
		cardinal temp(*this);
		int i;
		for (i = BIT_SIZE - FRACTIONAL_SIZE - 1; bits[i] && i >= 0; i--)
		{
			bits.set_at(i, false);
		}
		if (i > 0)
		{
			bits.set_at(i, true);
		}
		return temp;
	}
	
	cardinal operator ++ () noexcept
	{
		int i;
		for (i = BIT_SIZE - FRACTIONAL_SIZE - 1; bits[i] && i >= 0; i--)
		{
			bits.set_at(i, false);
		}
		if (i > 0)
		{
			bits.set_at(i, true);
		}
		return *this;
	}

	cardinal operator -- (int) noexcept
	{
		cardinal temp(*this);
		int i;
		for (i = BIT_SIZE - FRACTIONAL_SIZE - 1; !bits[i] && i >= 0; i--)
		{
			bits.set_at(i, true);
		}
		if (i > 0)
		{
			bits.set_at(i, false);
		}
		return temp;
	}

	cardinal operator -- () noexcept
	{
		int i;
		for (i = BIT_SIZE - FRACTIONAL_SIZE - 1; !bits[i] && i >= 0; i--)
		{
			bits.set_at(i, true);
		}
		if (i > 0)
		{
			bits.set_at(i, false);
		}
		return *this;
	}

	explicit operator int() const noexcept
	{
		if (bits[SIGN])
		{
			return -int(inverted().bits.right);
		}
		return int(bits.right);
	}

	explicit operator unsigned int() const noexcept
	{
		if (bits[SIGN])
		{
			return unsigned int(inverted().bits.right);
		}
		return unsigned int(bits.right);
	}

	explicit operator long long() const noexcept
	{
		if (bits[SIGN])
		{
			return -long long(inverted().bits.right);
		}
		return long long(bits.right);
	}

	explicit operator unsigned long long() const noexcept
	{
		if (bits[SIGN])
		{
			return inverted().bits.right;
		}
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
		for (i = BIT_SIZE - 1; bits[i] && i >= 0; --i)
		{
			bits.set_at(i, false);
		}
		if (i > 0)
		{
			bits.set_at(i, true);
		}
	}
	
	void decrement() noexcept /* substruct smallest amount */
	{
		int i;
		for (i = BIT_SIZE - 1; !bits[i] && i >= 0; i--)
		{
			bits.set_at(i, true);
		}
		if (i > 0)
		{
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

	const cardinal inverted() const
	{
		cardinal result(*this);
		result.invert();
		return result;
	}

	const bool get_bit(const unsigned int& position) const
	{
		return bits[position];
	}

	void set_bit(const unsigned int& position, const bool& value = false)
	{
		bits.set_at(position, value);
	}
};

#define CARDINAL_NUMBER

#endif // CARDINAL_NUMBER