#pragma once
#include "Globals.h"

#include <vector>

struct Resource {
	Resource() = default;
};

class ResourceManager
{
public:
	ResourceManager() {}
	virtual ~ResourceManager() {};

	virtual void Load(UID id, const char* libraryPath) = 0;
	virtual void Unload(UID id) = 0;

	virtual void Remove(UID id) = 0;

	virtual void Import(const char* assetsPath, const char* libraryPath) = 0;
	virtual void Save(UID id, const char* libraryPath) = 0;
};

// -----------------------------------
template <typename T>
class ResourceContainer
{
public:
	ResourceContainer() {}
	virtual ~ResourceContainer() {}

	int LoadResource(UID id, T& resource)
	{
		int index = FindResource(id);

		if (index == -1) // if resource not found create new one
		{
			index = ids.size();

			ids.push_back(id);
			refCounts.push_back(0);
			resources.push_back(T());
		}

		refCounts[index]++;
		resource = resources[index];

		return refCounts[index];
	}

	bool UnloadResource(UID id)
	{
		int index = FindResource(id);

		if (index == -1)
		{
			LOG("Error unloading resource, id not found: %d", id);
			return false;
		}

		refCounts[index]--;

		if (refCounts[index] > 0)
			return false;

		return true;
	}

	void RemoveResource(UID id)
	{
		int index = FindResource(id);

		if (index == -1)
		{
			LOG("Error removing resource, id not found: %d", id);
			return;
		}

		ids.erase(ids.begin() + index);
		refCounts.erase(refCounts.begin() + index);
		resources.erase(resources.begin() + index);
	}

	bool GetResource(UID id, T& resource)
	{
		int index = FindResource(id);
		if (index == -1)
		{
			LOG("Error resource id not found: %d", id);
			return false;
		}

		resource = resources[index];
		return true;
	}

	bool GetResource(UID id, const T& resource) const
	{
		int index = FindResource(id);
		if (index == -1)
		{
			LOG("Error resource id not found: %d", id);
			return false;
		}

		resource = resources[index];
		return true;
	}

	size_t GetSize() const 
	{ 
		return ids.size();
	}

private:
	int FindResource(UID id) const
	{
		for (size_t i = 0; i < ids.size(); ++i)
		{
			if (ids[i] == id)
				return i;
		}
		return -1;
	}

protected:
	std::vector<UID> ids;
	std::vector<unsigned int> refCounts;

	std::vector<T> resources;
};
