#pragma once

#include "DescriptorRegistry.h"
#include "StringTable.h"
#include "ItemBlockCache.h"
#include "History.h"
#include "Clipboard.h"

#include <ViewDesign/view/widget/DefaultWindow.h>
#include <ViewDesign/view/widget/TabView.h>
#include <ViewDesign/messaging/context.h>
#include <ViewDesign/common/holder.h>

#include <unordered_map>


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
		item_block_cache(data.root_item_block.get_manager()),
		root_item_block(std::move(data.root_item_block)) {}
private:
	BlockCacheDynamic common_cache;
private:
	DescriptorRegistry descriptor_registry;
	StringTable string_table;
	ItemBlockCache item_block_cache;
protected:
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
		tab_view = new TabView(std::move(root_item_block))
	), ContextProvider(AsViewBase()) {}

private:
	History history;
public:
	History& GetHistory() { return history; }

private:
	class TabView : public ViewDesign::TabView {
	public:
		TabView(item_block_ref root_ref) : ViewDesign::TabView(
			TabView::Style(),
			Tab(new TabView::DefaultHeaderFixed(u"clipboard"), new Clipboard())
		) {
			OpenRootItemBlockTab(std::move(root_ref));
		}
	private:
		std::unordered_map<ref_ptr<HeaderFrame>, ref_t> tab_ref_map;
		std::unordered_map<ref_t, ref_ptr<HeaderFrame>> ref_tab_map;
	private:
		virtual void OnTabClose(HeaderFrame& tab) override {
			if (auto it = tab_ref_map.find(&tab); it != tab_ref_map.end()) {
				ref_tab_map.erase(it->second);
				tab_ref_map.erase(it);
			}
			ViewDesign::TabView::OnTabClose(tab);
		}
	private:
		static std::u16string to_u16string(ref_t ref) {
			std::string s = std::to_string(ref);
			return std::u16string(s.begin(), s.end());
		}
	public:
		void OpenItemBlockTab(item_block_ref ref) {
			if (auto it = ref_tab_map.find(ref); it != ref_tab_map.end()) {
				it->second->Focus();
			} else {
				HeaderFrame& tab = Append(Tab(new TabView::DefaultHeaderClosable(u"#" + to_u16string(ref)), new ItemBlock(std::move(ref))));
				tab_ref_map.emplace(&tab, ref);
				ref_tab_map.emplace(ref, &tab);
			}
		}
	private:
		void OpenRootItemBlockTab(item_block_ref ref) {
			HeaderFrame& tab = Append(Tab(new TabView::DefaultHeaderFixed(u"root (#" + to_u16string(ref) + u")"), new ItemBlock(std::move(ref))));
			tab_ref_map.emplace(&tab, ref);
		}
	};
private:
	ref_ptr<TabView> tab_view;
public:
	void OpenItemBlockTab(item_block_ref ref) { tab_view->OpenItemBlockTab(std::move(ref)); }
};
