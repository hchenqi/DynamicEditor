#pragma once

#include "Clipboard.h"
#include "ItemBlock.h"

#include "TabView.h"

#include <ViewDesign/view/widget/DefaultWindow.h>


class MainWindow : public DefaultWindow {
public:
	MainWindow(item_block_ref root_item_block) : DefaultWindow(
		DefaultWindow::Style(),
		u"DynamicEditor",
		new TabView(
			TabView::Tab(new TabView::FixedHeader(u"clipboard"), new Clipboard::CreateView()),
			TabView::Tab(new TabView::FixedHeader(u"root item block"), new ItemBlock(std::move(root_item_block))),
		)
	) {}

public:
	void OpenTab(item_block_ref ref) {

	}
};
