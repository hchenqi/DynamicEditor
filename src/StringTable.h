#pragma once

#include <BlockStore/item/OrderedRefSet.h>
#include <BlockStore/utility/type_map.h>


using namespace BlockStore;


class StringTable {
public:
	using StringCache = BlockCache<std::string>;

private:
	using CacheMap = TypeMap<
		TypeMapEntry<std::string, StringCache>
	>;

	template<class T>
	using Cache = MappedTypeOr<CacheMap, T, BlockCacheDynamicAdapter<T>>;

private:
	StringCache string_cache;
	OrderedRefSet<std::u16string, Cache> set;

public:
	StringTable(BlockCacheDynamic common_cache, block_ref ref) : string_cache(ref.get_manager()), set(common_cache, common_cache, string_cache, std::move(ref)) {}

public:
	StringCache& GetStringCache() { return string_cache; }
	block<std::u16string> Insert(std::u16string str) { return set.insert(std::move(str)).drop(); }
};
