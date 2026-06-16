#pragma once

#include "Clipboard.h"
#include "ItemBlockCache.h"
#include "StringTable.h"

#include <ViewDesign/common/holder.h>
#include <ViewDesign/messaging/context.h>
#include <ViewDesign/view/widget/DefaultWindow.h>
#include <ViewDesign/view/widget/TabView.h>


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
	Meta(Data data) : descriptor_registry(std::move(data.descriptor_registry)), string_table(std::move(data.string_table)), root_item_block(std::move(data.root_item_block)) {}
private:
	DescriptorRegistry descriptor_registry;
	StringTable string_table;
	ItemBlockCache item_block_cache;
	item_block_ref root_item_block;
public:
	DescriptorRegistry& GetDescriptorRegistry() { return descriptor_registry; }
	StringTable& GetStringTable() { return string_table; }
	ItemBlockCache& GetItemBlockCache() { return item_block_cache; }
};


class MainWindow : public Meta, public DefaultWindow, private ContextProvider {
public:
	MainWindow(block_ref root) : Meta(std::move(root)), DefaultWindow(
		DefaultWindow::Style(),
		u"DynamicEditor",
		tab_view = new TabView(
			TabView::Tab(new TabView::DefaultHeaderFixed(u"clipboard"), new Clipboard())
		)
	), ContextProvider(AsViewBase()) {
		tab_view->Append(TabView::Tab(new TabView::DefaultHeaderFixed(u"root item block"), new ItemBlock(root_item_block)));
	}

private:
	ref_ptr<TabView> tab_view;
public:
	void OpenTab(item_block_ref ref) {

	}
};
