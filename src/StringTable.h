#pragma once

#include <BlockStore/item/OrderedRefSet.h>
#include <BlockStore/utility/type_map.h>


using namespace BlockStore;


class StringTable {
public:
	using StringCache = BlockCache<std::u16string>;

private:
	using CacheMap = TypeMap<
		TypeMapEntry<std::u16string, StringCache>
	>;

	template<class T>
	using Cache = MappedTypeOr<CacheMap, T, BlockCacheDynamicAdapter<T>>;

private:
	StringCache string_cache;
	OrderedRefSet<std::u16string, Cache> set;

public:
	StringTable(BlockCacheDynamic common_cache, block_ref ref) : string_cache(ref.get_manager()), set(common_cache, common_cache, string_cache, std::move(ref)) {}

public:
	block_view<std::u16string, StringCache> LookUp(block<std::u16string> ref) { return string_cache.read(std::move(ref)); }
	block_view<std::u16string, StringCache> Insert(std::u16string str) { return set.insert(std::move(str)); }
};
