#pragma once

#include "Item.h"
#include "MainWindow.h"


class ItemBlockRef : public Item {
public:
	static const Type type;
public:
	virtual Type GetType() const override { return type; }

private:
	static ItemBlockCache& GetItemBlockCache() {}

public:
	ItemBlockRef(item_block_ref ref) : ref(std::move(ref)) {}
	ItemBlockRef(DeserializeContext& context) : ItemBlockRef(context.access<item_block_ref>()) {}

private:
	item_block_ref ref;

public:
	class View : public Item::View {


	public:
		void Open() {

		}
		void Expand() {

		}
		void Collapse() {

		}
	};
};
