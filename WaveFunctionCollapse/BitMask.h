#pragma once

class BitMask
{
private:
	unsigned int numBits;
	unsigned int size;
	unsigned char* bitArray;

public:
	// --- Constructors
	BitMask();
	BitMask(unsigned int numBits);
	BitMask(const char* bits);
	BitMask(const BitMask& mask);

	// --- Destructor
	~BitMask();

	// --- Operators
	bool operator[](unsigned int index);
	const bool operator[](unsigned int index) const;
	const bool operator>(const BitMask& other) const;
	const bool operator==(const BitMask& other) const;
	const bool operator!=(const BitMask& other) const;
	BitMask& operator&=(const BitMask& other);

	// --- Utils
	void SetBit(unsigned int index, bool value);
	void SetAll(); // Set all bits to 1
	void ClearAll(); // Set all bits to 0

	int Count() const;
	int Pick(); // Returns index or -1 if no bit is set

	void Peek() const; 

private:
	const int GetByte(unsigned int index) const;
	const int GetBitPosition(unsigned int index) const;

	const bool GetBit(unsigned int index) const;
	void SetBit(unsigned int index);
	void ClearBit(unsigned int index);
};

