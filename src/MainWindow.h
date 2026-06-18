#pragma once

#include "Meta.h"
#include "History.h"
#include "ItemBlock.h"

#include <ViewDesign/view/widget/TabView.h>

#include <unordered_map>

#include <ViewDesign/view/widget/DefaultWindow.h>


class MainWindow : public DefaultWindow, private ContextProvider {
public:
	MainWindow(block_ref root) : DefaultWindow(
		DefaultWindow::Style(),
		u"DynamicEditor",
		meta_context = new MetaContext(
			std::move(root),
			new HistoryContext(
				tab_view = new MainTabView()
			)
		)
	), ContextProvider(AsViewBase()) {
		tab_view->OpenRootItemBlockTab(std::move(meta_context->GetRootItemBlock()));
	}

private:
	ref_ptr<MetaContext> meta_context;

private:
	class MainTabView : public TabView {
	private:
		friend class MainWindow;
	private:
		MainTabView() : TabView(TabView::Style()) {}
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
		void OpenItemBlockTab(const item_block_ref& ref) {
			if (auto it = ref_tab_map.find(ref); it != ref_tab_map.end()) {
				it->second->Focus();
			} else {
				HeaderFrame& tab = Append(Tab(new TabView::DefaultHeaderClosable(u"#" + to_u16string(ref)), new ItemBlock(ref)));
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
	ref_ptr<MainTabView> tab_view;
public:
	void OpenItemBlockTab(const item_block_ref& ref) { tab_view->OpenItemBlockTab(ref); }
};
