#pragma once

#include "item_block_ref.h"

#include <vector>


class History {
public:
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

public:
	void OnItemBlockUpdate(item_block_ref ref, ) {
		
	}

public:
	void BeginOperation() {
		undo_stack.emplace_back();
		
	}
	void CancelOperation() {
		
	}
	void EndOperation() {

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
