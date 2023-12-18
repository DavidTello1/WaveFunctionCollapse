#pragma once
#include <assert.h>
#include <xutility>

template <typename T>
class DynArray
{
private:
	unsigned int memCapacity = 0;
	unsigned int numElements = 0;
	T* data = nullptr;

	static constexpr int ARRAY_BLOCK_SIZE = 16;

public:
	// Default Constructor
	DynArray() : memCapacity(0), numElements(0), data(nullptr)
	{
		Alloc(ARRAY_BLOCK_SIZE);
	}

	// Parametrized Constructor
	DynArray(unsigned int capacity) : memCapacity(0), numElements(0), data(nullptr)
	{
		Alloc(capacity);
	}

	// Copy Constructor
	DynArray(const DynArray<T>& other)
	{
		memCapacity = other.memCapacity;
		numElements = other.numElements;
		data = (T*)::operator new(memCapacity * sizeof(T));

		for (unsigned int i = 0; i < numElements; ++i)
		{
			new(&data[i]) T(other.data[i]);
		}
	}
	
	// Move Constructor
	DynArray(DynArray<T>&& other) noexcept
	{
		memCapacity = other.memCapacity;
		numElements = other.numElements;
		data = other.data;
		other.data = nullptr;
	}
	
	// Destructor
	~DynArray()
	{
		clear();
		::operator delete(data, memCapacity * sizeof(T));
	}

	// Copy Assignment Operator
	const DynArray<T>& operator =(const DynArray<T>& other)
	{
		if (this == &other)
			return (*this);

		if (data != nullptr)
		{
			for (unsigned int i = 0; i < numElements; ++i)
				data[i].~T();
			::operator delete(data, memCapacity * sizeof(T));
		}

		memCapacity = other.memCapacity;
		numElements = other.numElements;
		data = (T*)::operator new(memCapacity * sizeof(T));

		for (unsigned int i = 0; i < numElements; ++i)
		{
			new(&data[i]) T(other.data[i]);
		}

		return (*this);
	}

	// Move Assignment Operator
	const DynArray<T>& operator =(DynArray<T>&& other) noexcept
	{
		if (this == &other)
			return (*this);

		if (data != nullptr)
		{
			for (unsigned int i = 0; i < numElements; ++i)
				data[i].~T();
			::operator delete(data, memCapacity * sizeof(T));
		}

		memCapacity = other.memCapacity;
		numElements = other.numElements;
		data = other.data;
		other.data = nullptr;

		return (*this);
	}

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

	// Add Element at the end
	void push_back(const T& element)
	{
		if (numElements >= memCapacity)
			Alloc(memCapacity + ARRAY_BLOCK_SIZE);

		new(&data[numElements]) T(element);
		numElements++;
	}

	// Add Element at the end (Move)
	void push_back(T&& element) noexcept
	{
		if (numElements >= memCapacity)
			Alloc(memCapacity + ARRAY_BLOCK_SIZE);

		new(&data[numElements]) T(std::move(element));
		numElements++;
	}

	// Emplace Back
	template <typename... Args> // Variadic Template
	T& emplace_back(Args&&... args) noexcept
	{
		if (numElements >= memCapacity)
			Alloc(memCapacity + ARRAY_BLOCK_SIZE);

		new(&data[numElements]) T(std::forward<Args>(args)...);
		numElements++;
		return data[numElements];
	}

	// Resize
	void resize(const unsigned int size)
	{
		Alloc(size);
	}

	// Erase
	void erase(int index) //*** needs testing
	{
		assert(index >= 0 && index < numElements);

		DynArray<T> tmp = DynArray<T>(memCapacity);

		for (unsigned int i = 0; i < numElements; ++i)
		{
			if (i == index)
			{
				data[i].~T();
				continue;
			}

			tmp.push_back(data[i]);
		}

		(*this).swap(tmp);
	}

	// Erase All Elements
	void clear()
	{
		if (data != nullptr)
		{
			for (unsigned int i = 0; i < numElements; ++i)
				data[i].~T();
		}
		numElements = 0;
	}

	// Erase Last Element
	void pop_back()
	{
		if (numElements <= 0)
			return;

		numElements--;
		data[numElements].~T();
	}

	// Swap
	void swap(DynArray<T>& other) //*** std::move without rvalue
	{
		DynArray<T> tmp = std::move(*this);
		*this = std::move(other);
		other = std::move(tmp);
	}
	
	// Fill
	void fill(const T& element)
	{
		clear();
		for (unsigned int i = 0; i < memCapacity; ++i)
		{
			new(&data[numElements]) T(element);
			numElements++;
		}
	}

	// Sort (from biggest to smallest) *CombSort
	DynArray<T>& sort()
	{
		DynArray<T>* sorted = this;

		static const float shrink = 1.3f;
		bool swapped = true;
		int gap = numElements - 1;

		while (swapped || gap > 1)
		{
			gap = (1 > gap / shrink) ? 1 : int(gap / shrink);

			swapped = false;
			for (unsigned int i = 0; i + gap < numElements; ++i)
			{
				if (sorted->data[i] < sorted->data[i + gap])
				{
					T tmp = std::move(sorted->data[i]);
					sorted->data[i] = std::move(sorted->data[i + gap]);
					sorted->data[i + gap] = std::move(tmp);

					swapped = true;
				}
			}
		}

		return (*sorted);
	}

	// Flip (reverse order)
	DynArray<T>& flip()
	{
		DynArray<T>* flipped = this;

		T* start = &flipped->front();
		T* end = &flipped->back();

		while (start < end)
		{
			T tmp = std::move(*start);
			*start = std::move(*end);
			*end = std::move(tmp);

			++start;
			--end;
		}

		return (*flipped);
	}


	// Capacity
	unsigned int capacity() const
	{
		return memCapacity;
	}

	// Size
	unsigned int size() const
	{
		return numElements;
	}

	// Empty
	unsigned int empty() const
	{
		return (numElements == 0);
	}

	// First Element
	T& front()
	{
		assert(numElements > 0);
		return data[0];
	}

	// First Element - Const
	const T& front() const
	{
		assert(numElements > 0);
		return data[0];
	}

	// Last Element
	T& back()
	{
		assert(numElements > 0);
		return data[numElements - 1];
	}

	// Last Element - Const
	const T& back() const
	{
		assert(numElements > 0);
		return data[numElements - 1];
	}

private:
	// Allocate Memory
	void Alloc(unsigned int mem)
	{
		T* tmp = (T*)::operator new(mem * sizeof(T)); //basically a malloc

		unsigned int tmpSize = (mem < numElements) ? mem : numElements;

		for (unsigned int i = 0; i < tmpSize; ++i) // fill tmp_array with data_array items
			new(&tmp[i]) T(std::move(data[i])); // create element in specified address

		for (unsigned int i = 0; i < numElements; ++i) // call destructors of items in data_array
			data[i].~T();
		::operator delete(data, memCapacity * sizeof(T)); // delete items in data_array (doesn't call destructors)

		memCapacity = mem;
		numElements = tmpSize;
		data = tmp;
	}
};
