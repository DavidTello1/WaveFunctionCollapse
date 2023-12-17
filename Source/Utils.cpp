#include "Utils.h"

#include "String.h"
#include "BitArray.h"

String Utils::MaskToString(const BitArray& mask)
{
	String string = String(mask.size());

	for (unsigned int i = 0; i < mask.size(); ++i)
	{
		string += (mask.getBit(i)) ? '1' : '0';
	}

	return string;
}
