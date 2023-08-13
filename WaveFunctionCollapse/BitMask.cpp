#include "BitMask.h"

#include "Globals.h"
#include "List.h"

#include <limits>
#include <random>
#include <time.h>

#include "mmgr/mmgr.h"

BitMask::BitMask()
{
	numBits = 0;
	size = 0;
	bitArray = nullptr;

	srand(time(NULL));
}

BitMask::BitMask(unsigned int numBits) : numBits(numBits) 
{
	//assert(numBits > 0);

	size = ((numBits - 1) / CHAR_BIT) + 1;
	bitArray = new unsigned char[size];

	ClearAll();
}

BitMask::BitMask(const char* bits)
{
	//assert(bits != nullptr && bits != "");

	numBits = strlen(bits);
	size = ((numBits - 1) / CHAR_BIT) + 1;
	bitArray = new unsigned char[size];

	for (int i = 0; i < numBits; ++i)
	{
		if (bits[i] == '1')
			SetBit(i);
		else
			ClearBit(i);
	}
}

BitMask::BitMask(const BitMask& mask)
{
	numBits = mask.numBits;
	size = mask.size;
	bitArray = new unsigned char[size];

	ClearAll();
	for (int i = 0; i < numBits; ++i)
	{
		if (mask.GetBit(i))
			SetBit(i);
	}
}

BitMask::~BitMask()
{
	delete[] bitArray;
	bitArray = nullptr;
}

bool BitMask::operator[](unsigned int index) //*** comprobar si funciona bien (deberia ser igual que GetBit())?
{
	//assert(index < numBits);

	int bit = index / CHAR_BIT;
	int position = 1 << (CHAR_BIT - 1 - ((index) % CHAR_BIT));

	return ((bitArray[bit] & position) != 0);
}

const bool BitMask::operator[](unsigned int index) const //*** igual que la anterior
{
	//assert(index < numBits);

	int bit = index / CHAR_BIT;
	int position = 1 << (CHAR_BIT - 1 - ((index) % CHAR_BIT));

	return ((bitArray[bit] & position) != 0);
}

const bool BitMask::operator>(const BitMask& other) const
{
	//assert(numBits != other.numBits);

	return (this->Count() > other.Count());
}

const bool BitMask::operator==(const BitMask& other) const
{
	if (numBits != other.numBits)
		return false;

	for (int i = 0; i < size; ++i)
	{
		if (bitArray[i] != other.bitArray[i])
			return false;
	}

	return true;
}

const bool BitMask::operator!=(const BitMask& other) const
{
	if (numBits != other.numBits)
		return true;

	for (int i = 0; i < size; ++i)
	{
		if (bitArray[i] != other.bitArray[i])
			return true;
	}

	return false;
}

BitMask& BitMask::operator&=(const BitMask& other)
{
	//assert(numBits == other.numBits);

	for (int i = 0; i < size; ++i)
	{
		bitArray[i] = bitArray[i] & other.bitArray[i];
	}

	return *this;
}

void BitMask::SetBit(unsigned int index, bool value)
{
	(value) ? SetBit(index) : ClearBit(index);
}


void BitMask::SetAll()
{
	for (int i = 0; i < numBits; i++)
		SetBit(i);
}

void BitMask::ClearAll()
{
	for (int i = 0; i < numBits; i++)
		ClearBit(i);
}

int BitMask::Count() const
{
	int count = 0;

	for (int i = 0; i < numBits; ++i)
	{
		if (GetBit(i) == true)
			++count;
	}

	return count;
}

int BitMask::Pick()
{
	int count = 0;
	List<int> positions = List<int>();

	for (int i = 0; i < numBits; ++i)
	{
		if (GetBit(i) == true)
		{
			positions.add(i);
			count++;
		}
	}

	if (count <= 0)
		return -1;

	int index = rand() % count;
	return positions[index]->data;
}

void BitMask::Peek() const
{
	for (int i = 0; i < numBits; ++i)
	{
		LOG("%d", (int)GetBit(i));
	}
}

// --------------------------------------
// --- Private Utils
const int BitMask::GetByte(unsigned int index) const
{
	return (index / CHAR_BIT);
}

const int BitMask::GetBitPosition(unsigned int index) const
{
	return (index % CHAR_BIT);
}

const bool BitMask::GetBit(unsigned int index) const
{
	int byte = index / CHAR_BIT;
	int pos = index % CHAR_BIT;

	return (bitArray[byte] & (1 << pos));
}

void BitMask::SetBit(unsigned int index)
{
	int byte = index / CHAR_BIT;
	int pos = index % CHAR_BIT;

	bitArray[byte] |= 1 << pos;
}

void BitMask::ClearBit(unsigned int index)
{
	int byte = index / CHAR_BIT;
	int pos = index % CHAR_BIT;

	bitArray[byte] &= ~(1 << pos);
}
