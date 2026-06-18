#pragma once

#include "item_block_ref.h"
#include "Item.h"

#include <vector>
#include <optional>
#include <stdexcept>


class History {
private:
	struct Entry {
		std::vector<std::pair<item_block_ref, ItemRef>> item_block_list;
	};
private:
	std::vector<Entry> undo_stack;
	std::vector<Entry> redo_stack;

private:
	struct OperationTemp {
		Entry entry;
	};
private:
	std::optional<OperationTemp> operation;
private:
	void CheckOperation() {
		if (!operation.has_value()) {
			throw std::logic_error("History: no ongoing operation");
		}
	}
public:
	void BeginOperation() {
		if (operation.has_value()) {
			throw std::logic_error("History: exists ongoing operation");
		}
		operation.emplace();
	}
	void OnItemBlockUpdate(item_block_ref ref, ItemRef item) {
		CheckOperation();
		operation->entry.item_block_list.emplace_back(std::move(ref), item);
	}
	void CancelOperation() {
		CheckOperation();
		operation.reset();
	}
	void EndOperation() {
		CheckOperation();
		undo_stack.emplace_back(std::move(operation->entry));
		operation.reset();
	}
public:
	void Operation(auto func) {
		BeginOperation();
		func();
		EndOperation();
	}

public:
	void Undo() {
		
	}
	void Redo() {

	}
};


#include <ViewDesign/view/frame/ViewFrame.h>
#include <ViewDesign/messaging/context.h>

using namespace ViewDesign;

class HistoryContext : public History, public ViewFrame, public SizeTrait<Fixed, Fixed>, private ContextProvider {
public:
	HistoryContext(view_ptr<Fixed, Fixed> child) : ViewFrame(std::move(child)), ContextProvider(AsViewBase()) {}
};
