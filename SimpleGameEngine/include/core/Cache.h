#pragma once

#include <iostream>
#include <unordered_map>
#include <tuple>
#include <string>
#include <utility>

namespace sg
{
	template <typename TKey, typename TValue>
	class Cache;

	template <typename TKey, typename TValue>
	class CacheRef
	{
	private:
		using CacheMap = std::unordered_map<TKey, std::tuple<TValue, int, bool>>;
	public:
		
		CacheRef<TKey, TValue>& operator=(const CacheRef<TKey, TValue>&) = delete;
		
		CacheRef<TKey, TValue>& operator=(CacheRef<TKey, TValue>&& other) = delete;

		CacheRef(const CacheRef<TKey, TValue>& ref) 
			: m_cache(ref.m_cache), key(nullptr), value(nullptr)
		{
			if (ref.validRef())
			{
				initialize((*ref.key), (*ref.value));
			}
		}

		CacheRef(CacheRef<TKey, TValue>&& ref) 
			: m_cache(ref.m_cache), key(nullptr), value(nullptr)
		{
			if (ref.validRef())
			{
				initialize((*ref.key), (*ref.value));
			}
		}

		CacheRef()
			: m_cache(nullptr), key(nullptr), value(nullptr)
		{

		}

		CacheRef(const TKey& valueKey, std::tuple<TValue, int, bool>& valueInfo, Cache<TKey, TValue>* cache)
			: m_cache(cache)
		{
			initialize(valueKey, valueInfo);
		}

		~CacheRef()
		{
			if (validRef())
			{
				m_cache->deleteReference(*key, *value);
			}
		}

		TValue& get() const { return std::get<0>(*value); }

		int getNumRef() const { return std::get<1>(*value); }

	private:
		bool validRef() const { return key && value; }

		void initialize(const TKey& key, std::tuple<TValue, int, bool>& info)
		{
			// Set the key and value
			this->key = &key;
			value = &info;

			// Increment ref count by one
			++std::get<1>(info);
		}

		Cache<TKey, TValue>* const m_cache;
		const TKey* key;
		std::tuple<TValue, int, bool>* value;
	};

	template <typename TKey, typename TValue>
	class Cache
	{
		using ValueInfo = std::tuple<TValue, int, bool>;
		using CacheMap = std::unordered_map<TKey, ValueInfo>;
		using CRef = CacheRef<TKey, TValue>;

	public:
		Cache() = default;
		~Cache() = default;
		Cache(const Cache&) = delete;
		Cache(Cache&&) = delete;
		Cache& operator=(const Cache&) = delete;
		Cache& operator=(Cache&&) = delete;

		CRef add(const TKey& key, const TValue& value, bool autoCacheDelete = true)
		{
			// Make sure TValue is not in the cache already
			typename CacheMap::iterator kvp = m_cache.find(key);

			if (kvp != m_cache.end())
			{
				return CRef(kvp->first, kvp->second, this);
			}
			else
			{
				// Insert in cache map
				ValueInfo val(value, 0, autoCacheDelete);
				std::pair<typename CacheMap::iterator, bool> insert = m_cache.insert(std::make_pair(key, val));
				typename CacheMap::iterator kvp = insert.first;

				// Return the newly inserted element as a CacheRef
				return CRef(kvp->first, kvp->second, this);
			}
		}
		
		std::pair<CRef, bool> find(const TKey& key)
		{
			typename CacheMap::iterator kvp = m_cache.find(key);

			if (kvp == m_cache.end())
			{
				return std::pair<CRef, bool>( CRef(), false );
			}
			else
			{
				return std::pair<CRef, bool>( CRef(kvp->first, kvp->second, this), true );
			}
		}

		void print()
		{
#ifdef _DEBUG
			for (auto element : m_cache)
			{
				std::cout << "Key: " << element.first << "\tValue:" << std::get<0>(element.second) << " #Ref: " << std::get<1>(element.second) << std::endl;
			}

			std::cout << std::endl;
#endif
		}
		
	private:
		//friend CacheRef<TKey, TValue>::~CacheRef();
		friend class CacheRef<TKey, TValue>;
		void deleteReference(const TKey& key, ValueInfo& value)
		{
			// If there is a match, decrement ref count by one 
			int& numRef = std::get<1>(value);
			--numRef;

			if (numRef == 0 && std::get<2>(value))
			{
				m_cache.erase(key);
			}
		}

		CacheMap m_cache;
	};
}
