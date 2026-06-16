#pragma once

#include "Item.h"


class ItemBlock : private block_ref, private Item::View {
public:
	ItemBlock(item_block_ref ref) : block_ref(std::move(ref)), item(std::make_unique) {

	}
	~ItemBlock() {}

private:
	ItemRef root;

private:
	virtual void OnChildUpdate(const View& child, std::unique_ptr<const Item> item) const {

	}
};
