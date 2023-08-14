#pragma once

// --- Double Linked List ---
// --------------------------

template<typename T>
struct ListItem 
{
	T data;
	ListItem<T>* next;
	ListItem<T>* prev;

	inline ListItem(const T& data) : data(data), next(nullptr), prev(nullptr) {}
	~ListItem() {}
};

template<typename T>
class List
{
private: 
	ListItem<T>* start;
	ListItem<T>* end;
	unsigned int numElements;

public:
	// --- Constructor
	inline List() {
		start = end = nullptr;
		numElements = 0;
	}

	// --- Destructor
	~List() {
		clear();
	}

	// --- Operators
	const List<T>& operator +=(const List<T>& other) {
		ListItem<T>* item = other.start;

		while (item != nullptr)
		{
			add(item->data);
			item = item->next;
		}
		return (*this);
	}

	const ListItem<T>* operator[](unsigned int index) const {
		//assert(index > 0 && index < numElements);

		ListItem<T>* item = start;
		for (unsigned int i = 0; i < index; ++i)
		{
			item = item->next;
		}
		return item;
	}

	// --- Data Management
	ListItem<T>* add(const T& item) {
		ListItem<T>* itemData;
		itemData = new ListItem<T>(item);

		if (start == nullptr)
			start = end = itemData;
		else
		{
			itemData->prev = end;
			end->next = itemData;
			end = itemData;
		}

		++numElements;
		return itemData;
	}

	bool erase(ListItem<T>* item)
	{
		if (item == 0)
			return false;

		// Now reconstruct the list
		if (item->prev != 0)
		{
			item->prev->next = item->next;

			if (item->next != 0)
				item->next->prev = item->prev;
			else
				end = item->prev;
		}
		else
		{
			if (item->next)
			{
				item->next->prev = 0;
				start = item->next;
			}
			else
				start = end = 0;
		}

		delete item;
		item = 0;
		--numElements;

		return true;
	}

	void clear() {
		ListItem<T>* data;
		ListItem<T>* next;
		data = start;

		while (data != nullptr)
		{
			next = data->next;
			delete(data);
			data = next;
		}

		start = end = nullptr;
		numElements = 0;
	}

	void pop_front() {
		ListItem<T>* item = start;

		if (item == nullptr)
			return;

		if (item->next != nullptr)
		{
			start = item->next;
		}
		else
		{
			start = end = nullptr;
		}

		delete item;
		item = nullptr;
		--numElements;
	}

	// --- Utils
	unsigned int size() const {
		return numElements;
	}

	bool empty() const {
		return (numElements == 0);
	}

	ListItem<T>* front() const {
		return start;
	}
};