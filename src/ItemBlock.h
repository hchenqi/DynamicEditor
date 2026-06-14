#pragma once

#include "Item.h"
#include "item_block_ref.h"

#include <BlockStore/data/cache.h>


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


class ItemBlockCache {
public:

};


class ItemBlockRef : public Item {
public:
	static const Type type;
public:
	virtual Type GetType() const override { return type; }

private:
	static ItemBlockCache& GetItemBlockCache() {}

public:
	ItemBlockRef(item_block_ref ref) : ref(std::move(ref)) {
		if (auto data = read(); data.empty()) {
			throw std::invalid_argument("block data uninitialized");
		} else {
			DeserializeContext context(get_manager(), std::move(data));
			item = ConstructChild(type, context);
		}
	}
	ItemBlockRef(interpreter_ref type, block_ref ref, std::function<std::unique_ptr<ItemView>()> init) : block_ref(std::move(ref)) {
		if (auto data = read(); data.empty()) {
			item = init();
			RegisterChild(*item);
			if (type != GetChildType(*item)) {
				throw std::invalid_argument("child type mismatch");
			}
			Serialize();
		} else {
			DeserializeContext context(get_manager(), std::move(data));
			item = ConstructChild(type, context);
		}
	}

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
