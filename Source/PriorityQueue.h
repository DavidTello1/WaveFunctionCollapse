#pragma once

template <typename T, bool max = true>
class PriorityQueue
{
private:
	T* data;
	unsigned int memCapacity;
	unsigned int numElements;

	static const unsigned int arrayBlockSize = 16;

public:
	// --- Constructors
	PriorityQueue() : memCapacity(0), numElements(0), data(nullptr) {
		Alloc(arrayBlockSize);
	}

	PriorityQueue(unsigned int capacity) : memCapacity(capacity), numElements(0), data(nullptr) {
		Alloc(capacity);
	}

	// --- Destructor
	~PriorityQueue() {
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
	void push(const T& element) {
		if (numElements >= memCapacity)
		{
			Alloc(memCapacity + arrayBlockSize);
		}

		for (unsigned int i = 0; i < numElements; ++i)
		{
			if (element > data[i] && max == true)
			{
				insert(element, i);
				return;
			}
			else if (element < data[i] && max == false)
			{
				insert(element, i);
				return;
			}
		}

		data[numElements++] = element;
	}

	void insert(const T& element, unsigned int position) {
		//assert(position < numElements);

		if (position == numElements)
		{
			push(element);
			return;
		}

		if (numElements + 1 > memCapacity)
			Alloc(memCapacity + arrayBlockSize);

		for (unsigned int i = numElements; i > position; --i)
		{
			data[i] = data[i - 1];
		}

		data[position] = element;
		++numElements;
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

private:
	// --- Private Utils
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