#pragma once
#include <assert.h>
#include <xutility>

// -----------------------------------
// -----------------------------------
// --- BIT-ARRAY CLASS ---
class BitArray
{
private:
	using WORD = unsigned char;

private:
	unsigned int numBits = 0;
	unsigned int numWords = 0;
	WORD* data = nullptr;

	static constexpr unsigned int DEFAULT_BITARRAY_SIZE = 16; // 2 bytes
	static constexpr unsigned int WORD_SIZE = sizeof(WORD) * 8;

public:
	// Default Constructor
	BitArray()
	{
		Alloc(DEFAULT_BITARRAY_SIZE);
		clearAll();
	}

	// Parametrized Constructor - Size
	BitArray(const unsigned int size)
	{
		Alloc(size);
		clearAll();
	}

	// Parametrized Constructor - Const Char*
	BitArray(const char* bits)
	{
		Alloc(strlen(bits));

		for (unsigned int i = 0; i < numBits; ++i)
		{
			(bits[i] == '1') ? setBit(i) : clearBit(i);
		}
	}

	// Copy Constructor
	BitArray(const BitArray& other)
	{
		numBits = other.numBits;
		numWords = other.numWords;
		data = new WORD[numWords];

		for (unsigned int i = 0; i < numWords; ++i)
		{
			data[i] = other.data[i];
		}
	}

	// Move Constructor
	BitArray(BitArray&& other) noexcept
	{
		numBits = other.numBits;
		numWords = other.numWords;
		data = other.data;
		other.data = nullptr;
	}

	// Destructor
	~BitArray()
	{		
		delete[] data;
	}

	// Copy Assignment Operator
	BitArray& operator =(const BitArray& other)
	{
		if (this == &other)
			return (*this);

		if (data != nullptr)
		{
			delete[] data;
		}

		numBits = other.numBits;
		numWords = other.numWords;
		data = new WORD[numWords];

		for (unsigned int i = 0; i < numWords; ++i)
		{
			data[i] = other.data[i];
		}

		return (*this);
	}

	// Move Assignment Operator
	BitArray& operator =(BitArray&& other) noexcept
	{
		if (this == &other)
			return (*this);

		if (data != nullptr)
		{
			delete[] data;
		}

		numBits = other.numBits;
		numWords = other.numWords;
		data = other.data;
		other.data = nullptr;

		return (*this);
	}

	// Assignment Operator - Const Char*
	BitArray& operator =(const char* bits)
	{
		if (bits == nullptr)
			return (*this);

		if (data != nullptr)
		{
			delete[] data;
		}

		Alloc(strlen(bits));
		for (unsigned int i = 0; i < numBits; ++i)
		{
			(bits[i] == '1') ? setBit(i) : clearBit(i);
		}

		return (*this);
	}

	// Equals Operator
	bool operator ==(const BitArray& other) const
	{
		if (numBits != other.numBits)
			return false;

		for (unsigned int i = 0; i < numBits; ++i)
		{
			if (getBit(i) != other.getBit(i))
				return false;
		}

		return true;
	}

	// Not Equals Operator
	bool operator !=(const BitArray& other) const
	{
		if (numBits != other.numBits)
			return true;

		for (unsigned int i = 0; i < numBits; ++i)
		{
			if (getBit(i) != other.getBit(i))
				return true;
		}

		return false;
	}

	// Greater Operator
	bool operator >(const BitArray& other) const
	{
		assert(numBits == other.numBits);
		return (count() > other.count());
	}

	// Bitwise AND Operator
	BitArray& operator &=(const BitArray& other)
	{
		assert(numBits == other.numBits);

		for (unsigned int i = 0; i < numBits; ++i)
		{
			(getBit(i) && other.getBit(i)) ? setBit(i) : clearBit(i);
		}

		return (*this);
	}

	// Bitwise OR Operator
	BitArray& operator |=(const BitArray& other)
	{
		assert(numBits == other.numBits);

		for (unsigned int i = 0; i < numBits; ++i)
		{
			(getBit(i) || other.getBit(i)) ? setBit(i) : clearBit(i);
		}

		return (*this);
	}

	// Size
	unsigned int size() const
	{
		return numBits;
	}

	// Get Bit
	bool getBit(const unsigned int index) const
	{
		int word = index / WORD_SIZE;
		int bit = index % WORD_SIZE;

		return (data[word] & (1 << bit));
	}

	// Set Bit
	void setBit(const unsigned int index)
	{
		int word = index / WORD_SIZE;
		int bit = index % WORD_SIZE;

		data[word] |= 1 << bit;
	}

	// Clear Bit
	void clearBit(const unsigned int index)
	{
		int word = index / WORD_SIZE;
		int bit = index % WORD_SIZE;

		data[word] &= ~(1 << bit);
	}

	// Set All Bits
	void setAll()
	{
		for (unsigned int i = 0; i < numBits; i++)
			setBit(i);
	}

	// Clear All Bits
	void clearAll()
	{
		for (unsigned int i = 0; i < numBits; i++)
			clearBit(i);
	}

	// Number of Set Bits
	int count() const
	{
		int counter = 0;

		for (unsigned int i = 0; i < numBits; ++i)
		{
			if (getBit(i) == true)
				counter++;
		}

		return counter;
	}

	int firstSetBit(int offset = 0) const
	{
		if (offset < 0 || offset >= numBits)
			return -1;

		for (unsigned int i = offset; i < numBits; ++i)
		{
			if (getBit(i) == true)
				return i;
		}

		return -1;
	}

private:
	// Allocate Memory
	void Alloc(unsigned int mem)
	{
		numBits = mem;

		numWords = numBits / WORD_SIZE;
		if (numBits % WORD_SIZE != 0)
			numWords++;

		data = new WORD[numWords];
	}
};
