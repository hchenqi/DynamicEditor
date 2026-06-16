#pragma once

#include <BlockStore/item/OrderedRefSet.h>


using namespace BlockStore;


class StringTable {
private:
	template<class T>
	struct CacheType {
		using Type = BlockCacheDynamicAdapter<T>;
	};

	template<>
	struct CacheType<std::u16string> {
		using Type = BlockCache<std::u16string>;
	};

	template<class T>
	using Cache = CacheType<T>::Type;

private:
	BlockCacheDynamic node_leaf_cache;
	BlockCache<std::u16string> key_cache;
	OrderedRefSet<std::u16string, Cache> set;

public:
	StringTable(block_ref ref) : node_leaf_cache(ref.get_manager()), key_cache(ref.get_manager()), set(node_leaf_cache, node_leaf_cache, key_cache, std::move(ref)) {}

public:
	


};
