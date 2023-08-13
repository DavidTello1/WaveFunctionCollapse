#pragma once

template<typename T>
class DynArray
{
private:
	T* data;
	unsigned int memCapacity;
	unsigned int numElements;

	static const unsigned int arrayBlockSize = 16;

public:
	// --- Constructors
	DynArray() : memCapacity(0), numElements(0), data(nullptr) {
		Alloc(arrayBlockSize);
	}

	DynArray(unsigned int capacity) : memCapacity(capacity), numElements(0), data(nullptr) {
		Alloc(capacity);
	}

	DynArray(const DynArray<T>& other) {

		memCapacity = other.memCapacity;
		numElements = other.numElements;
		data = new T[memCapacity];

		for (unsigned int i = 0; i < numElements; ++i)
			data[i] = other.data[i];
	}

	// --- Destructor
	~DynArray() {
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

	// --- Data Management
	void push_back(const T& element) {
		if (numElements >= memCapacity)
		{
			Alloc(memCapacity + arrayBlockSize);
		}
		data[numElements++] = element;
	}

	void clear() {
		numElements = 0;
	}

	T& front() {
		//assert(numElements > 0);
		return data[0];
	}

	// --- Utils
	unsigned int capacity() const {
		return memCapacity;
	}

	unsigned int size() const {
		return numElements;
	}

	bool empty() const {
		return (numElements == 0);
	}

	//DynArray<T> sort() { // Comb-Sort
	//	DynArray<T> ret = *this;

	//	bool swapped = true;
	//	int gap = ret.numElements - 1;
	//	float shrink = 1.3f;

	//	while (swapped || gap > 1)
	//	{
	//		gap = (1 > gap / shrink) ? 1 : gap / shrink;

	//		swapped = false;
	//		for (unsigned int i = 0; i + gap < ret.numElements - 1; ++i)
	//		{
	//			if (ret.data[i] > ret.data[i + gap])
	//			{
	//				T tmp = ret.data[i];
	//				ret.data[i] = ret.data[i + gap];
	//				ret.data[i + gap] = tmp;

	//				swapped = true;
	//			}
	//		}
	//	}
	//	return ret;
	//}

private:
	// --- Private Utils //*** MOVE SEMANTICS (avoid copying data to tmp and then back to data)
	void Alloc(unsigned int mem)
	{
		T* tmp = data;

		memCapacity = mem;
		numElements = (memCapacity < numElements) ? memCapacity : numElements;
		data = new T[memCapacity];

		if (tmp != nullptr)
		{
			for (unsigned int i = 0; i < numElements; ++i)
				data[i] = tmp[i];

			delete[] tmp;
		}
	}
};