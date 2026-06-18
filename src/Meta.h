#pragma once

#include "DescriptorRegistry.h"
#include "StringTable.h"
#include "item_block_ref.h"


class Meta {
private:
	struct Data {
		block_ref descriptor_registry;
		block_ref string_table;
		item_block_ref root_item_block;

		friend constexpr auto layout(layout_type<Data>) { return declare(&Data::descriptor_registry, &Data::string_table, &Data::root_item_block); }
	};
public:
	Meta(block_ref root) : Meta(static_cast<block<Data>&>(root).read([&]() {
		auto& manager = root.get_manager();
		return Data{ manager.allocate(), manager.allocate(), manager.allocate() };
	})) {}
private:
	Meta(Data data) :
		common_cache(data.root_item_block.get_manager()),
		descriptor_registry(common_cache, std::move(data.descriptor_registry)),
		string_table(common_cache, std::move(data.string_table)),
		root_item_block(std::move(data.root_item_block)) {}
private:
	BlockCacheDynamic common_cache;
	DescriptorRegistry descriptor_registry;
	StringTable string_table;
	item_block_ref root_item_block;
public:
	DescriptorRegistry& GetDescriptorRegistry() { return descriptor_registry; }
	StringTable& GetStringTable() { return string_table; }
	const item_block_ref& GetRootItemBlock() { return root_item_block; }
};


#include <ViewDesign/view/frame/ViewFrame.h>
#include <ViewDesign/messaging/context.h>

using namespace ViewDesign;

class MetaContext : public Meta, public ViewFrame, public SizeTrait<Fixed, Fixed>, private ContextProvider {
public:
	MetaContext(block_ref root, view_ptr<Fixed, Fixed> child) : Meta(std::move(root)), ViewFrame(std::move(child)), ContextProvider(AsViewBase()) {}
};
