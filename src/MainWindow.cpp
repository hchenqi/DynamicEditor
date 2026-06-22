#include "MainWindow.h"
#include "ItemBlock.h"


void MainWindow::TabView::OpenRootItemBlockTab(const item_block_ref& ref) {
	ref_ptr<ItemBlock> item_block;
	ref_ptr<HeaderFrame> tab = &Append(Tab(new TabView::DefaultHeaderFixed(u"root (#" + to_u16string(ref) + u")"), item_block = new ItemBlock(ref, main_window.GetMeta())));
	tab_ref_map.emplace(tab, ref);
	ref_tab_map.emplace(ref, std::make_pair(tab, item_block));
	tab->Focus();
}

ItemBlock& MainWindow::TabView::OpenItemBlockTab(const item_block_ref& ref) {
	if (auto it = ref_tab_map.find(ref); it != ref_tab_map.end()) {
		auto [tab, item_block] = it->second;
		tab->Focus();
		return *item_block;
	} else {
		ref_ptr<ItemBlock> item_block;
		ref_ptr<HeaderFrame> tab = &Append(Tab(new TabView::DefaultHeaderClosable(u"#" + to_u16string(ref)), item_block = new ItemBlock(ref, main_window.GetMeta())));
		tab_ref_map.emplace(tab, ref);
		ref_tab_map.emplace(ref, std::make_pair(tab, item_block));
		tab->Focus();
		return *item_block;
	}
}
