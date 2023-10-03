#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#define TMP_STRING_SIZE	4096

class String
{
private:
	char* str;
	unsigned int size;

public:
	// --- Constructors
	String() {
		Alloc(1);
		clear();
	}

	String(unsigned int size) {
		(size > 0) ? Alloc(size) : Alloc(1);
		clear();
	}

	String(const String& string) {
		Alloc(string.size);
		strcpy_s(str, size, string.str);
	}

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

	// --- Destructor
	virtual ~String() {
		if (str != nullptr)
			delete[] str;
	}

	// --- Operators
	bool operator== (const String& string) const
	{
		return strcmp(string.str, str) == 0;
	}

	bool operator== (const char* string) const
	{
		if (string != nullptr)
			return strcmp(string, str) == 0;

		return false;
	}

	bool operator!= (const String& string) const
	{
		return strcmp(string.str, str) != 0;
	}

	bool operator!= (const char* string) const
	{
		if (string != nullptr)
			return strcmp(string, str) != 0;

		return true;
	}

	const String& operator= (const String& string)
	{
		if (string.length() + 1 > size)
		{
			delete[] str;
			Alloc(string.length() + 1);
		}
		else
			clear();

		strcpy_s(str, size, string.str);

		return(*this);
	}

	const String& operator= (const char* string)
	{
		String t(string);
		(*this) = t;
		return (*this);
	}

	const String& operator+= (const String& string)
	{
		unsigned int need_size = string.length() + length() + 1;

		if (need_size > size)
		{
			char* tmp = str;
			Alloc(need_size);
			strcpy_s(str, size, tmp);
			delete[] tmp;
		}

		strcat_s(str, size, string.str);

		return(*this);
	}

	const String& operator+= (const char* string)
	{
		if (string != nullptr)
		{
			unsigned int need_size = strlen(string) + length() + 1;

			if (need_size > size)
			{
				char* tmp = str;
				Alloc(need_size);
				strcpy_s(str, size, tmp);
				delete[] tmp;
			}

			strcat_s(str, size, string);
		}

		return(*this);
	}

	// --- Utils
	const char* c_str() const {
		return str;
	}

	unsigned int length() const {
		return strlen(str);
	}

	unsigned int capacity() const {
		return size;
	}

	void clear() {
		str[0] = '\0';
	}

	const String& create(const char* format, ...) { // same as constructor but it returns a reference to itself
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

		return *this;
	}

	unsigned find(const char* string) const
	{
		unsigned ret = 0;

		if (string != nullptr)
		{
			unsigned len = strlen(string);

			for (unsigned i = 0; i < size - len; ++i)
			{
				if (strncmp(string, &str[i], len) == 0)
				{
					i += len;
					++ret;
				}
			}
		}

		return ret;
	}

private:
	void Alloc(unsigned int mem)
	{
		size = mem;
		str = new char[size];
	}
};
