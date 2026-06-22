#pragma once

#include "item_block_ref.h"
#include "ItemRef.h"

#include <vector>
#include <optional>


class MainWindow;

class History {
public:
	History(MainWindow& main_window);
	~History();

private:
	MainWindow& main_window;

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
	void CheckOperation();
public:
	void BeginOperation();
	void OnItemBlockUpdate(item_block_ref ref, ItemRef item);
	void CancelOperation();
	void EndOperation();
public:
	void Operation(auto func) {
		BeginOperation();
		func();
		EndOperation();
	}

public:
	void Undo();
	void Redo();
};
