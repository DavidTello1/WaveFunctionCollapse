#pragma once
#include <assert.h>

// -----------------------------
// --- STACK ALLOCATED ARRAY ---
// -----------------------------

template <typename T, unsigned int numElements>
class StaticArray
{
private:
	T data[numElements];

public:
	// Member Access Operator
	T& operator [](unsigned int index) 
	{
		assert(index < numElements);
		return data[index];
	}

	// Member Access Operator - Const
	const T& operator [](unsigned int index) const 
	{
		assert(index < numElements);
		return data[index];
	}

	// Get Size
	constexpr int size() const 
	{
		return numElements;
	}

	// Get Internal Array
	T* get_data()
	{
		return data;
	}

	// Get Internal Array - Const
	const T* get_data() const
	{
		return data;
	}

	// Fill Array
	void fill(const T& value)
	{
		for (unsigned int i = 0; i < numElements; ++i)
			data[i] = value;
	}
};
