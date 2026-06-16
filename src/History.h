#pragma once

#include "item_block_ref.h"

#include <vector>
#include <optional>
#include <stdexcept>


class History {
private:
	struct Entry {
		struct ItemBlock {
			item_block_ref ref;
			ItemRef item;
		};

		std::vector<ItemBlock> item_block_list;
		
	};
private:
	std::vector<Entry> undo_stack;
	std::vector<Entry> redo_stack;

private:
	struct Operation {
		Entry entry;
		// edit state
	};
private:
	std::optional<Operation> operation;

public:
	void OnItemBlockUpdate(item_block_ref ref, ) {

	}

public:
	void BeginOperation() {
		if (operation.has_value()) {
			throw std::logic_error("History: exists ongoing operation");
		}
		operation.emplace();
	}
	void CancelOperation() {

	}
	void EndOperation() {
		undo_stack
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
