#pragma once
#include <assert.h>
#include <xutility>

// -----------------------------------
//     --- DOUBLY-LINKED LIST ---
// -----------------------------------

// -----------------------------------
// --- LIST ITEM ---
template<typename T>
struct ListItem
{
public:
	T data;
	ListItem<T>* next = nullptr;
	ListItem<T>* prev = nullptr;

public:
	ListItem(const T& data) : data(data), next(nullptr), prev(nullptr) {}
	ListItem(T&& data) noexcept : data(std::move(data)), next(nullptr), prev(nullptr) {}
	~ListItem() {}
};

// -----------------------------------
// --- LIST ITERATOR ---
template <typename T>
class ListIterator
{
public:
	using ValueType = typename T::ValueType;
	using PointerType = ValueType*;
	using ReferenceType = ValueType&;

private:
	ListItem<ValueType>* ptr;

public:
	ListIterator(ListItem<ValueType>* ptr) : ptr(ptr) {}

	ListIterator& operator++() {
		ptr = ptr->next;
		return (*this);
	}

	ListIterator operator++(int) {
		ListIterator it = *this;
		++(*this);
		return it;
	}

	ListIterator& operator+(int offset) {
		while (offset > 0)
		{
			ptr = ptr->next;
			offset--;
		}
		return (*this);
	}

	ListIterator& operator--() {
		ptr = ptr->prev;
		return (*this);
	}

	ListIterator operator--(int) {
		ListIterator it = *this;
		--(*this);
		return it;
	}

	ListIterator& operator-(int offset) {
		while (offset > 0)
		{
			ptr = ptr->prev;
			offset--;
		}
		return (*this);
	}

	bool operator==(const ListIterator& other) const {
		return ptr == other.ptr;
	}

	bool operator!=(const ListIterator& other) const {
		return !(*this == other);
	}

	ReferenceType operator[](int index) {
		return *((*this) + index);
	}

	PointerType operator->() const {
		return &ptr->data;
	}

	ReferenceType operator*() const {
		return (*ptr).data;
	}

	ListItem<ValueType>* data() const {
		return ptr;
	}
};

// -----------------------------------
// --- LIST ITERATOR CONST ---
template <typename T>
class ListIteratorConst
{
public:
	using ValueType = typename T::ValueType;
	using PointerType = const ValueType*;
	using ReferenceType = const ValueType&;

private:
	ListItem<ValueType>* ptr;

public:
	ListIteratorConst(ListItem<ValueType>* ptr) : ptr(ptr) {}

	ListIteratorConst& operator++() {
		ptr = ptr->next;
		return (*this);
	}

	ListIteratorConst operator++(int) {
		ListIteratorConst it = *this;
		++(*this);
		return it;
	}

	ListIteratorConst& operator+(int offset) {
		while (offset > 0)
		{
			ptr = ptr->next;
			offset--;
		}
		return (*this);
	}

	ListIteratorConst& operator--() {
		ptr = ptr->prev;
		return (*this);
	}

	ListIteratorConst operator--(int) {
		ListIteratorConst it = *this;
		--(*this);
		return it;
	}

	ListIteratorConst& operator-(int offset) {
		while (offset > 0)
		{
			ptr = ptr->prev;
			offset--;
		}
		return (*this);
	}

	bool operator==(const ListIteratorConst& other) const {
		return ptr == other.ptr;
	}

	bool operator!=(const ListIteratorConst& other) const {
		return !(*this == other);
	}

	ReferenceType operator[](int index) {
		return *((*this) + index);
	}

	PointerType operator->() const {
		return &ptr->data;
	}

	ReferenceType operator*() const {
		return (*ptr).data;
	}

	const ListItem<ValueType>& data() const {
		return *ptr;
	}
};

// -----------------------------------
// --- LIST ITERATOR REVERSE ---
template <typename T>
class ListIteratorReverse
{
public:
	using ValueType = typename T::ValueType;
	using PointerType = ValueType*;
	using ReferenceType = ValueType&;

private:
	ListItem<ValueType>* ptr;

public:
	ListIteratorReverse(ListItem<ValueType>* ptr) : ptr(ptr) {}

	ListIteratorReverse& operator++() {
		ptr = ptr->prev;
		return (*this);
	}

	ListIteratorReverse operator++(int) {
		ListIteratorReverse it = *this;
		++(*this);
		return it;
	}

	ListIteratorReverse& operator+(int offset) {
		while (offset > 0)
		{
			ptr = ptr->next;
			offset--;
		}
		return (*this);
	}

	ListIteratorReverse& operator--() {
		ptr = ptr->next;
		return (*this);
	}

	ListIteratorReverse operator--(int) {
		ListIteratorReverse it = *this;
		--(*this);
		return it;
	}

	ListIteratorReverse& operator-(int offset) {
		while (offset > 0)
		{
			ptr = ptr->prev;
			offset--;
		}
		return (*this);
	}

	bool operator==(const ListIteratorReverse& other) const {
		return ptr == other.ptr;
	}

	bool operator!=(const ListIteratorReverse& other) const {
		return !(*this == other);
	}

	ReferenceType operator[](int index) {
		return *((*this) + index);
	}

	PointerType operator->() const {
		return &ptr->data;
	}

	ReferenceType operator*() const {
		return (*ptr).data;
	}

	ListItem<ValueType>& data() const {
		return *ptr;
	}
};

// -----------------------------------
// --- LIST ITERATOR REVERSE CONST ---
template <typename T>
class ListIteratorReverseConst
{
public:
	using ValueType = typename T::ValueType;
	using PointerType = const ValueType*;
	using ReferenceType = const ValueType&;

private:
	ListItem<ValueType>* ptr;

public:
	ListIteratorReverseConst(ListItem<ValueType>* ptr) : ptr(ptr) {}

	ListIteratorReverseConst& operator++() {
		ptr = ptr->prev;
		return (*this);
	}

	ListIteratorReverseConst operator++(int) {
		ListIteratorReverseConst it = *this;
		++(*this);
		return it;
	}

	ListIteratorReverseConst& operator+(int offset) {
		while (offset > 0)
		{
			ptr = ptr->next;
			offset--;
		}
		return (*this);
	}

	ListIteratorReverseConst& operator--() {
		ptr = ptr->next;
		return (*this);
	}

	ListIteratorReverseConst operator--(int) {
		ListIteratorReverseConst it = *this;
		--(*this);
		return it;
	}

	ListIteratorReverseConst& operator-(int offset) {
		while (offset > 0)
		{
			ptr = ptr->prev;
			offset--;
		}
		return (*this);
	}

	bool operator==(const ListIteratorReverseConst& other) const {
		return ptr == other.ptr;
	}

	bool operator!=(const ListIteratorReverseConst& other) const {
		return !(*this == other);
	}

	ReferenceType operator[](int index) {
		return *((*this) + index);
	}

	PointerType operator->() const {
		return &ptr->data;
	}

	ReferenceType operator*() const {
		return (*ptr).data;
	}

	const ListItem<ValueType>& data() const {
		return *ptr;
	}
};

// -----------------------------------
// -----------------------------------
// --- LIST CLASS ---
template <typename T>
class List
{
public:
	using ValueType = T;

	using Iterator = ListIterator<List<T>>;
	using IteratorConst = ListIteratorConst<List<T>>;
	using IteratorReverse = ListIteratorReverse<List<T>>;
	using IteratorReverseConst = ListIteratorReverseConst<List<T>>;

private:
	ListItem<T>* first = nullptr;
	ListItem<T>* last = nullptr;
	unsigned int numElements = 0;

public:
	// Default Constructor
	List() : first(nullptr), last(nullptr), numElements(0)
	{
	}

	// Copy Constructor
	List(const List<T>& other)
	{
		append(other);
	}

	// Move Constructor
	List(List<T>&& other) noexcept
	{
		first = other.first;
		last = other.last;
		numElements = other.numElements;

		other.first = nullptr;
		other.last = nullptr;
	}

	// Destructor
	~List()
	{
		clear();
	}

	// Copy Assignment Operator
	const List<T>& operator =(const List<T>& other)
	{
		if (this == &other)
			return (*this);

		if (numElements > 0)
			clear();

		append(other);

		return (*this);
	}

	// Move Assignment Operator
	const List<T>& operator =(List<T>&& other) noexcept
	{
		if (this == &other)
			return (*this);

		if (numElements > 0)
				clear();

		first = other.first;
		last = other.last;
		numElements = other.numElements;

		other.first = nullptr;
		other.last = nullptr;

		return (*this);
	}

	// Add Element at the end
	void append(const T& element)
	{
		ListItem<T>* item = new ListItem<T>(element);

		if (first == nullptr)
			first = last = item;
		else
		{
			item->prev = last;
			last->next = item;
			last = item;
		}
		++numElements;
	}

	// Add Element at the end - Move
	void append(T&& element) noexcept
	{
		ListItem<T>* item = new ListItem<T>(std::move(element));

		if (first == nullptr)
			first = last = item;
		else
		{
			item->prev = last;
			last->next = item;
			last = item;
		}
		++numElements;
	}

	// Add various Elements at the end
	void append(const List<T>& elements)
	{
		for (unsigned int i = 0; i < elements.numElements; ++i)
			append(elements.at(i));
	}

	// Add various Elements at the end - Move
	void append(List<T>&& elements)
	{
		for (unsigned int i = 0; i < elements.numElements; ++i)
			append(std::move(elements.at(i)));

		elements.first = nullptr;
		elements.last = nullptr;
		elements.numElements = 0;
	}

	// Erase Element by ListItem
	void erase(Iterator iter)
	{
		ListItem<T>* item = iter.data();

		// Reconstruct the list
		if (item->prev != 0)
		{
			item->prev->next = item->next;

			if (item->next != 0)
				item->next->prev = item->prev;
			else
				last = item->prev;
		}
		else
		{
			if (item->next)
			{
				item->next->prev = 0;
				first = item->next;
			}
			else
				first = last = 0;
		}

		delete item;
		item = nullptr;
		--numElements;
	}

	// Erase Element by element
	bool erase(const T& element)
	{
		if (numElements == 0)
			return false;

		for (Iterator it = first; it != last->next; ++it)
		{
			if ((*it) == element)
			{
				erase(it);
				return true;
			}
		}
		return false;
	}

	// Erase Element at index
	bool eraseAt(const unsigned int index)
	{
		assert(index < numElements);
		return erase(begin() + index);
	}

	// Erase All Elements
	void clear()
	{
		ListItem<T>* next;
		ListItem<T>* data = first;

		while (data != nullptr)
		{
			next = data->next;

			delete data;
			data = next;
		}

		first = last = nullptr;
		numElements = 0;
	}

	// Erase First Element
	void pop_front()
	{
		assert(numElements > 0);

		ListItem<T>* data = first;

		first = first->next;

		if (first != nullptr)
			first->prev = nullptr;

		numElements--;
		delete data;
	}

	// Erase Last Element
	void pop_back()
	{
		assert(numElements > 0);

		ListItem<T>* data = last;

		last = last->prev;
		last->next = nullptr;
		numElements--;

		delete data;
	}

	// Get Element at index
	T& at(const unsigned int index)
	{
		assert(index < numElements);

		ListItem<T>* item = first;
		for (unsigned int i = 0; i < index; ++i)
		{
			item = item->next;
		}
		return item->data;
	}

	// Get Element at index - Const
	T& at(const unsigned int index) const
	{
		assert(index < numElements);

		ListItem<T>* item = first;
		for (unsigned int i = 0; i < index; ++i)
		{
			item = item->next;
		}
		return item->data;
	}

	// Check if Element is inside list
	bool contains(const T& element) const
	{
		for (ListItem<T>* item = first; item != nullptr; item = item->next)
		{
			if (element == item->data)
				return true;
		}
		return false;
	}

	// Get Element index (returns -1 if not found)
	int find(const T& element) const
	{
		int count = 0;
		for (ListItem<T>* item = first; item != nullptr; item = item->next, count++)
		{
			if (element == item->data)
				return count;
		}
		return -1;
	}

	// Size
	unsigned int size() const
	{
		return numElements;
	}

	// Empty
	bool empty() const
	{
		return (numElements == 0);
	}

	// Get First Element
	T& front()
	{
		assert(first != nullptr);
		return first->data;
	}

	// Get First Element - Const
	const T& front() const
	{
		assert(first != nullptr);
		return first->data;
	}

	// Get Last Element
	T& back()
	{
		assert(last != nullptr);
		return last->data;
	}

	// Get Last Element - Const
	const T& back() const
	{
		assert(end != nullptr);
		return last->data;
	}

	// Iterator - Begin
	Iterator begin() const
	{
		return Iterator(first);
	}

	// Iterator - End
	Iterator end() const
	{
		return Iterator(last->next);
	}

	// Iterator - Const Begin
	IteratorConst cbegin() const
	{
		return IteratorConst(first);
	}

	// Iterator - Const End
	IteratorConst cend() const
	{
		return IteratorConst(last->next);
	}
	
	// Iterator - Reverse Begin
	IteratorReverse rbegin() const
	{
		return IteratorReverse(last);
	}
	
	// Iterator - Reverse End
	IteratorReverse rend() const
	{
		return IteratorReverse(first->prev);
	}

	// Iterator - Const Reverse Begin
	IteratorReverseConst crbegin() const
	{
		return IteratorReverseConst(last);
	}

	// Iterator - Const Reverse End
	IteratorReverseConst crend() const
	{
		return IteratorReverseConst(first->prev);
	}
};
