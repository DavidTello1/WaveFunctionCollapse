#pragma once
#include <iosfwd>
#include <stdarg.h>
#include <xutility>

#include <assert.h>

#include <string>

class String
{
private:
	char* str = nullptr;
	unsigned int size = 0;

	static constexpr char END_CHAR = '\0';
	static constexpr int TMP_STRING_SIZE = 4096;

public:
	// Default Constructor
	String()
	{
		Alloc(1);
		clear();
	}

	// Parametrized Constructor - Size
	String(unsigned int size)
	{
		(size > 0) ? Alloc(size) : Alloc(1);
		clear();
	}

	// Parametrized Constructor - Char
	String(char c)
	{
		Alloc(2);
		str[0] = c;
		str[1] = END_CHAR;
	}

	// Parametrized Constructor - Const Char*
	String(const char* format, ...)
	{
		size = 0;

		if (format != nullptr)
		{
			static char tmp[TMP_STRING_SIZE];
			static va_list  ap;

			// Construct the string from variable arguments
			va_start(ap, format);
			int res = vsprintf_s(tmp, TMP_STRING_SIZE, format, ap);
			va_end(ap);

			if (res > 0)
			{
				Alloc(res + 1);
				strcpy_s(str, size, tmp);
			}
		}

		if (size == 0)
		{
			Alloc(1);
			clear();
		}
	}

	// Copy Constructor
	String(const String& other)
	{
		Alloc(other.size);
		strcpy_s(str, size, other.str);
	}

	// Move Constructor
	String(String&& other) noexcept
	{
		size = other.size;
		str = other.str;
		other.str = nullptr;
	}

	// Destructor
	~String()
	{
		if (str != nullptr)
			delete[] str;
	}

	// Copy Assignment Operator
	String& operator =(const String& other)
	{
		if (other.length() + 1 > size)
		{
			delete[] str;
			Alloc(other.length() + 1);
		}
		else
			clear();

		strcpy_s(str, size, other.str);

		return(*this);
	}

	// Move  Assignment Operator
	String& operator =(String&& other) noexcept
	{
		if (this == &other)
			return (*this);

		size = other.size;
		str = other.str;
		other.str = nullptr;

		return (*this);
	}

	// Assignment Operator (const char*)
	String& operator =(const char* other)
	{
		if (*this == other)
			return (*this);

		if (strlen(other) + 1 > size)
		{
			delete[] str;
			Alloc(strlen(other) + 1);
		}
		else
			clear();

		strcpy_s(str, size, other);

		return (*this);
	}

	// Operator Equals
	bool operator ==(const String& other) const
	{
		return strcmp(other.str, str) == 0;
	}

	// Operator Equals (const char*)
	bool operator ==(const char* other) const
	{
		if (other == nullptr)
			return false;
		
		return strcmp(other, str) == 0;		
	}

	// Operator Not Equals
	bool operator !=(const String& other) const
	{
		return strcmp(other.str, str) != 0;
	}

	// Operator Not Equals (const char*)
	bool operator !=(const char* other) const
	{
		if (other == nullptr)
			return true;

		return strcmp(other, str) != 0;
	}

	// Addition Operator
	String operator +(const String& other)
	{
		unsigned int thisLength = length();
		unsigned int otherLength = other.length();
		unsigned int totalLength = thisLength + otherLength + 1;

		String tmp(totalLength);

		for (unsigned int i = 0; i < thisLength; ++i)
			tmp.str[i] = str[i];

		for (unsigned int i = 0; i < otherLength; ++i)
			tmp.str[i + thisLength] = other.str[i];

		tmp.str[totalLength - 1] = END_CHAR;

		return tmp;
	}

	// Addition Operator (char)
	String operator +(char other)
	{
		unsigned int totalLength = length() + 2;
		String tmp(totalLength);

		strcpy_s(tmp.str, totalLength, str);

		tmp.str[totalLength - 2] = other;
		tmp.str[totalLength - 1] = END_CHAR;

		return tmp;
	}

	// Addition Operator (const char*)
	String operator +(const char* other)
	{
		unsigned int totalLength = length() + strlen(other) + 1;
		String tmp(totalLength);

		strcpy_s(tmp.str, totalLength, str);
		strcat_s(tmp.str, totalLength, other);

		return tmp;
	}

	// Addition Assignment Operator
	String& operator +=(const String& other)
	{
		unsigned int totalLength = length() + other.length() + 1;

		if (totalLength > size)
		{
			char* tmp = str;
			Alloc(totalLength);
			strcpy_s(str, size, tmp);
			delete[] tmp;
		}

		strcat_s(str, size, other.str);

		return (*this);
	}

	// Addition Assignment Operator (char)
	String& operator +=(char other)
	{
		unsigned int totalLength = length() + 2;

		if (totalLength > size)
		{
			char* tmp = str;
			Alloc(totalLength);
			strcpy_s(str, size, tmp);
			delete[] tmp;
		}

		str[totalLength - 2] = other;
		str[totalLength - 1] = END_CHAR;

		return (*this);
	}

	// Addition Assignment Operator (const char*)
	String& operator +=(const char* other)
	{
		if (other == nullptr)
			return (*this);
		
		unsigned int totalLength = length() + strlen(other) + 1;

		if (totalLength > size)
		{
			char* tmp = str;
			Alloc(totalLength);
			strcpy_s(str, size, tmp);
			delete[] tmp;
		}

		strcat_s(str, size, other);

		return (*this);
	}

	// To const char*
	const char* c_str() const
	{
		return str;
	}

	// Length
	unsigned int length() const
	{
		return strlen(str);
	}

	// Capacity
	unsigned int capacity() const
	{
		return size;
	}

	// Clear
	void clear()
	{
		str[0] = END_CHAR;
	}

	// Contains
	bool contains(const char* string) const
	{
		if (string == nullptr)
			return false;

		unsigned int len = strlen(string);

		for (unsigned int i = 0; i < size - len; ++i)
		{
			if (strncmp(string, &str[i], len) == 0)
				return true;
		}

		return false;
	}

	// Find
	// returns index to first match, -1 if not found
	// startIndex offsets the starting point of the search, useful for finding all matches
	int find(const char* string, unsigned int startIndex = 0) const
	{
		if (string == nullptr)
			return -1;

		unsigned int len = strlen(string);

		for (unsigned int i = startIndex; i < size - len; ++i)
		{
			if (strncmp(string, &str[i], len) == 0)
				return i;
		}

		return -1;
	}

	// Count number of occurrences of string
	int count(const char* string, unsigned int startIndex = 0) const
	{
		if (string == nullptr)
			return 0;

		unsigned int len = strlen(string);

		int count = 0;
		for (unsigned int i = startIndex; i < size - len; ++i)
		{
			if (strncmp(string, &str[i], len) == 0)
			{
				i += len;
				count++;
			}
		}

		return count;
	}

	// Replace
	String replace(const char* replacing, const char* replacement)
	{
		if (replacing == nullptr)
			return (*this);

		if (replacement == nullptr)
			replacement = "";

		int index = find(replacing);
		if (index == -1)
			return (*this);

		int replacingLength = strlen(replacing);
		int replacementLength = strlen(replacement);
		int lengthDiff = replacementLength - replacingLength;
		int totalLength = length() + lengthDiff + 1;

		char* tmp = new char[totalLength];

		int i = 0;
		for (; i < index; ++i)
			tmp[i] = str[i];

		for (; i < index + replacementLength; ++i)
			tmp[i] = replacement[i - index];

		for (; i < totalLength; ++i)
			tmp[i] = str[i - lengthDiff];

		tmp[totalLength  - 1] = END_CHAR;

		return String(tmp);
	}

	// ReplaceAll
	String replaceAll(const char* replacing, const char* replacement) //***
	{	 
		if (replacing == nullptr)
			return (*this);

		if (replacement == nullptr)
			replacement = "";

		int* indices = new int[length()];
		int numFound = 0;

		// Find occurrencies and save indices
		int replacingLength = strlen(replacing);
		for (unsigned int i = 0; i < size - replacingLength; ++i)
		{
			if (strncmp(replacing, &str[i], replacingLength) == 0)
			{
				indices[numFound] = i;
				numFound++;
			}
		}

		if (numFound == 0)
			return (*this);

		// Create tmp char*
		int replacementLength = strlen(replacement);
		int lengthDiff = replacementLength - replacingLength;
		int totalLength = length() + numFound * lengthDiff + 1;

		char* tmp = new char[totalLength];

		// Fill tmp with replaced string
		int counter = 0;
		int prevIndex = 0;
		while (counter < numFound) 
		{
			int index = indices[counter];

			int i = prevIndex;
			for (; i < index; ++i)
			{
				tmp[i] = str[i - counter * lengthDiff];
			}

			for (int j = 0; j < replacementLength; ++j)
			{
				tmp[i + j] = replacement[j];
			}

			prevIndex = index + replacementLength;
			counter++;
		}

		// Complete the string with the remaining chars after the last replacement
		for (int i = prevIndex; i < totalLength; ++i)
		{
			tmp[i] = str[i - counter * lengthDiff];
		}

		tmp[totalLength] = END_CHAR;
		delete[] indices;

		return String(tmp);
	}

	// Substr
	String substr(unsigned int index, unsigned int len)
	{
		if (index > length() || len == 0)
			return String();

		char* tmp = new char[len + 1];

		for (int i = 0; i < len; ++i)
		{
			tmp[i] = str[index + i];
		}
		tmp[len] = END_CHAR;

		String string = tmp;
		delete[] tmp;

		return string;
	}

private:
	void Alloc(unsigned int mem)
	{
		size = mem;
		str = new char[size];
	}
};
