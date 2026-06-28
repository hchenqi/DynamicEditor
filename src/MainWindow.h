#pragma once

#include "Meta.h"
#include "History.h"

#include <ViewDesign/view/widget/DefaultWindow.h>
#include <ViewDesign/view/widget/TabView.h>
#include <ViewDesign/view/wrapper/Background.h>
#include <ViewDesign/common/holder.h>
#include <ViewDesign/messaging/context.h>

#include <unordered_map>


using namespace ViewDesign;


class ItemBlock;

class MainWindow : private Holder<Meta>, private Holder<History>, public DefaultBackground<DefaultWindow>, private ContextProvider {
public:
	MainWindow(block_ref root) :
		Holder<Meta>(std::move(root)),
		Holder<History>(*this),
		Base(
			DefaultWindow::Style(),
			u"DynamicEditor",
			tab_view = new TabView(*this)
		),
		ContextProvider(AsViewBase()) {
		tab_view->OpenRootItemBlockTab(GetMeta().GetRootItemBlock());
	}

public:
	Meta& GetMeta() { return Holder<Meta>::value; }
	History& GetHistory() { return Holder<History>::value; }

private:
	class TabView : public ViewDesign::TabView {
	public:
		TabView(MainWindow& main_window) : ViewDesign::TabView(Style()), main_window(main_window) {}
	private:
		MainWindow& main_window;
	private:
		std::unordered_map<ref_ptr<HeaderFrame>, ref_t> tab_ref_map;
		std::unordered_map<ref_t, std::pair<ref_ptr<HeaderFrame>, ref_ptr<ItemBlock>>> ref_tab_map;
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
		void OpenRootItemBlockTab(const item_block_ref& ref);
		ItemBlock& OpenItemBlockTab(const item_block_ref& ref);
	};
private:
	ref_ptr<TabView> tab_view;
public:
	ItemBlock& OpenItemBlockTab(const item_block_ref& ref) { return tab_view->OpenItemBlockTab(ref); }
};
