#include "History.h"
#include "Item.h"
#include "ItemBlock.h"

#include <stdexcept>


History::History(MainWindow& main_window) : main_window(main_window) {}

History::~History() {}

void History::CheckOperation() {
	if (!operation.has_value()) {
		throw std::logic_error("History: no ongoing operation");
	}
}

void History::BeginOperation() {
	if (operation.has_value()) {
		throw std::logic_error("History: exists ongoing operation");
	}
	operation.emplace();
}

void History::OnItemBlockUpdate(item_block_ref ref, ItemRef item) {
	CheckOperation();
	operation->entry.item_block_list.emplace_back(std::move(ref), std::move(item));
}

void History::CancelOperation() {
	CheckOperation();
	operation.reset();
}

void History::EndOperation() {
	CheckOperation();
	redo_stack.clear();
	undo_stack.emplace_back(std::move(operation->entry));
	operation.reset();
}

void History::Undo() {
	if (undo_stack.empty()) { return; }
	Entry entry = std::move(undo_stack.back()); undo_stack.pop_back();
	for (auto& [ref, item] : entry.item_block_list) {
		item = main_window.OpenItemBlockTab(ref).SetRoot(std::move(item));
	}
	redo_stack.push_back(std::move(entry));
}

void History::Redo() {
	if (redo_stack.empty()) { return; }
	Entry entry = std::move(redo_stack.back()); redo_stack.pop_back();
	for (auto& [ref, item] : entry.item_block_list) {
		item = main_window.OpenItemBlockTab(ref).SetRoot(std::move(item));
	}
	undo_stack.push_back(std::move(entry));
}
