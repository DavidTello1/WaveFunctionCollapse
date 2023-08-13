#pragma once

template <typename T, unsigned int numElements>
class StaticArray
{
private:
	T* data;

public:
	// --- Constructor
	inline StaticArray() {
		data = new T[numElements];
	}

	// --- Destructor
	~StaticArray() {
		delete[] data;
		data = nullptr;
	}

	// --- Operators
	T& operator[](unsigned int index) {
		//assert(index < numElements);
		return data[index];
	}

	const T& operator[](unsigned int index) const {
		//assert(index < numElements);
		return data[index];
	}

	// --- Utils
	unsigned int size() const {
		return numElements;
	}
};