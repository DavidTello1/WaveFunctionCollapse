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

	numBits = (unsigned int)strlen(bits);
	size = ((numBits - 1) / CHAR_BIT) + 1;
	bitArray = new unsigned char[size];

	for (unsigned int i = 0; i < numBits; ++i)
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
	for (unsigned int i = 0; i < numBits; ++i)
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

	for (unsigned int i = 0; i < size; ++i)
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

	for (unsigned int i = 0; i < size; ++i)
	{
		if (bitArray[i] != other.bitArray[i])
			return true;
	}

	return false;
}

BitMask& BitMask::operator&=(const BitMask& other)
{
	//assert(numBits == other.numBits);

	for (unsigned int i = 0; i < size; ++i)
	{
		bitArray[i] = bitArray[i] & other.bitArray[i];
	}

	return *this;
}

BitMask& BitMask::operator|=(const BitMask& other)
{
	//assert(numBits == other.numBits);

	for (unsigned int i = 0; i < size; ++i)
	{
		bitArray[i] = bitArray[i] | other.bitArray[i];
	}

	return *this;
}

const char* BitMask::ToString() //***
{
	char text[50] = ""; //*** size equals numBits

	for (unsigned int i = 0; i < numBits; ++i)
	{
		(GetBit(i)) ? strcat_s(text, "1") : strcat_s(text, "0");
	}

	return text;
}

void BitMask::SetBit(unsigned int index, bool value)
{
	(value) ? SetBit(index) : ClearBit(index);
}


void BitMask::SetAll()
{
	for (unsigned int i = 0; i < numBits; i++)
		SetBit(i);
}

void BitMask::ClearAll()
{
	for (unsigned int i = 0; i < numBits; i++)
		ClearBit(i);
}

int BitMask::Count() const
{
	int count = 0;

	for (unsigned int i = 0; i < numBits; ++i)
	{
		if (GetBit(i) == true)
			++count;
	}

	return count;
}

List<unsigned int> BitMask::GetSetBits()
{
	List<unsigned int> positions = List<unsigned int>();

	for (unsigned int i = 0; i < numBits; ++i)
	{
		if (GetBit(i) == true)
			positions.add(i);
	}

	return positions;
}

int BitMask::Pick()
{
	List<unsigned int> positions = GetSetBits();

	if (positions.size() == 0)
		return -1;

	int index = rand() % positions.size();

	int pick = positions[index]->data;
	//ClearAll();
	//SetBit(pick);

	return pick;
}

void BitMask::Peek() const
{
	for (unsigned int i = 0; i < numBits; ++i)
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
