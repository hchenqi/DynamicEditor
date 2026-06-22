#include "History.h"
#include "Item.h"

#include <stdexcept>


History::History() {}

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
	operation->entry.item_block_list.emplace_back(std::move(ref), item);
}

void History::CancelOperation() {
	CheckOperation();
	operation.reset();
}

void History::EndOperation() {
	CheckOperation();
	undo_stack.emplace_back(std::move(operation->entry));
	operation.reset();
}

void History::Undo() {}

void History::Redo() {}
