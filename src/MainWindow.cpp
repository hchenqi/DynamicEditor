#include "MainWindow.h"
#include "ItemBlock.h"


void MainWindow::TabView::OpenItemBlockTab(const item_block_ref& ref) {
	if (auto it = ref_tab_map.find(ref); it != ref_tab_map.end()) {
		it->second->Focus();
	} else {
		HeaderFrame& tab = Append(Tab(new TabView::DefaultHeaderClosable(u"#" + to_u16string(ref)), new ItemBlock(ref, main_window.GetMeta())));
		tab_ref_map.emplace(&tab, ref);
		ref_tab_map.emplace(ref, &tab);
	}
}

void MainWindow::TabView::OpenRootItemBlockTab(const item_block_ref& ref) {
	HeaderFrame& tab = Append(Tab(new TabView::DefaultHeaderFixed(u"root (#" + to_u16string(ref) + u")"), new ItemBlock(ref, main_window.GetMeta())));
	tab_ref_map.emplace(&tab, ref);
}
