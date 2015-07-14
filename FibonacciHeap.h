/*
 * FibonacciHeap.h
 *
 *  Created on: 06.12.2014
 *      Author: domenicjenz
 */

#pragma once

#include <vector>
#include <exception>
#include <map>
#include <ostream>

namespace Utilities
{

template <typename KeyType, typename ValueType>
struct TreeNode
{
	std::size_t getRank () const
	{
		return children.size();
	}

	KeyType key;
	ValueType value;
	std::vector<TreeNode<KeyType, ValueType>*> children;
};

template <typename KeyType, typename ValueType>
std::ostream& operator<< (std::ostream& os, const TreeNode<KeyType, ValueType>& victim)
{
	os << "[";
	os << victim.key << ", " << victim.value;
	std::size_t numberOfChildren = victim.children.size();
	for (unsigned int i = 0; i < numberOfChildren; ++i)
	{
		os << ", " << *(victim.children[i]);
	}
	os << "]";
	return os;
}


template <typename KeyType, typename ValueType>
class FibonacciHeap
{
using TreeType = TreeNode<KeyType, ValueType>;
public:
	FibonacciHeap () = default;
	virtual ~FibonacciHeap () = default;

	bool isEmpty () const
	{
		return (_numberOfEntries == 0);
	}

	void insert (const KeyType& key, const ValueType& value)
	{
		TreeType* newTree = new TreeType;
		newTree->key = key;
		newTree->value = value;
		_fiboWood.insert (std::make_pair(0, newTree));
		++_numberOfEntries;
	}

	std::pair<KeyType,ValueType> getMin ()
	{
		if (isEmpty ())
		{
			throw std::length_error("getMin on empty FibonacciHeap was called");
		}
		TreeType* minTree = _fiboWood.begin().second;
		for (TreeType* entry : _fiboWood)
		{
			if (minTree->key < entry->key)
			{
				minTree = entry;
			}
		}
		std::pair<KeyType, ValueType> result (minTree->key, minTree->value);
		return result;
	}

	std::pair<KeyType,ValueType> deleteMin ()
	{
		if (isEmpty ())
		{
			throw std::length_error("deleteMin on empty FibonacciHeap was called");
		}
		auto currentIt = _fiboWood.begin();
		auto minIt = _fiboWood.begin();
		auto endIt = _fiboWood.end();
		for (auto currentIt = _fiboWood.begin(); currentIt != endIt; ++currentIt)
		{
			if (currentIt->second->key < minIt->second->key)
			{
				minIt = currentIt;
			}
		}
		TreeType* minTree = minIt->second;
		if ((_maxRank > 0) && (minIt->first == _maxRank))
		{
			--_maxRank;
		}
		_fiboWood.erase (minIt);
		for (TreeType* subTree : minTree->children)
		{
			_fiboWood.insert (std::make_pair(subTree->getRank(), subTree));
		}
		std::pair<KeyType, ValueType> result (minTree->key, minTree->value);
		delete minTree;
		--_numberOfEntries;
		mergeWood();
		return result;
	}

	//friend std::ostream& operator<< (std::ostream& os, const FibonacciHeap<KeyType, ValueType>& victim);

	void print ()
	{
		for (const auto& rankTrees : _fiboWood)
		{
			std::cout << rankTrees.first << ": " << *(rankTrees.second) << std::endl;
		}
	}

private:
	std::size_t _numberOfEntries = 0;
	std::size_t _maxRank = 0;
	std::multimap<std::size_t, TreeType*> _fiboWood;

	void mergeWood ()
	{
		bool mergeHappened = false;
		for (std::size_t rank = 0; rank <= _maxRank; ++rank)
		{
			mergeHappened = mergeRank (rank);
		}
		while (mergeHappened)
		{
			++_maxRank;
			mergeHappened = mergeRank (_maxRank);
		}
	}

	TreeType* mergeTrees (TreeType* first, TreeType* second)
	{
		//std::cout << "merge " << *first << " with " << *second << std::endl;
		if (first->key < second->key)
		{
			first->children.push_back(second);
			return first;
		}
		else
		{
			second->children.push_back(first);
			return second;
		}
	}

	bool mergeRank (std::size_t rank)
	{
		auto startAndEnd = _fiboWood.equal_range(rank);
		bool mergeHappened = false;
		std::vector<TreeType*> mergedTrees;
		if (startAndEnd.first == startAndEnd.second)
		{
			return false;
		}
		TreeType* first = nullptr;
		TreeType* second = nullptr;
		unsigned int counter = 0;
		for (auto currentIt = startAndEnd.first; currentIt != startAndEnd.second; ++currentIt)
		{
			if ((counter % 2) == 0)
			{
				first = currentIt->second;
			}
			else
			{
				second = currentIt->second;
				TreeType* mergeResult = mergeTrees (first, second);
				mergedTrees.push_back (mergeResult);
				mergeHappened = true;
				first = nullptr;
			}
			++counter;
		}
		_fiboWood.erase (rank);
		for (TreeType* mergedTree : mergedTrees)
		{
			_fiboWood.insert (std::make_pair(rank+1, mergedTree));
		}

		if (first != nullptr)
		{
			_fiboWood.insert (std::make_pair (rank, first));
		}
		return mergeHappened;
	}
};


} /* namespace Utilities */


