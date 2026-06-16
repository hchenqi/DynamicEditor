#pragma once

#include <BlockStore/item/OrderedRefSet.h>
#include <BlockStore/utility/type_map.h>
#include <CppSerialize/stl/variant.h>


struct DescriptorType;

using descriptor_ref = block<DescriptorType>;

using ItemDescriptorType = size_t;
using TupleDescriptorType = std::vector<descriptor_ref>;
using DynamicLengthArrayDescriptorType = descriptor_ref;

struct DescriptorType : std::variant<ItemDescriptorType, TupleDescriptorType, DynamicLengthArrayDescriptorType> {
	using layout_base = std::variant<ItemDescriptorType, TupleDescriptorType, DynamicLengthArrayDescriptorType>;
};


class DescriptorRegistry {
public:
	using DescriptorCache = BlockCache<DescriptorType>;

private:
	using CacheMap = TypeMap<
		TypeMapEntry<DescriptorType, DescriptorCache>
	>;

	template<class T>
	using Cache = MappedTypeOr<CacheMap, T, BlockCacheDynamicAdapter<T>>;

private:
	DescriptorCache descriptor_cache;
	OrderedRefSet<DescriptorType, Cache> set;

public:
	DescriptorRegistry(BlockCacheDynamic common_cache, block_ref ref) : descriptor_cache(ref.get_manager()), set(common_cache, common_cache, descriptor_cache, std::move(ref)) {}

public:
	block_view<DescriptorType, DescriptorCache> LookUp(descriptor_ref ref) { return descriptor_cache.read(std::move(ref)); }
	descriptor_ref Insert(DescriptorType descriptor) { return set.insert(std::move(descriptor)).drop(); }
};
