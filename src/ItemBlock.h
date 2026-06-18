#pragma once

#include "item_block_ref.h"
#include "Item.h"
#include "History.h"
#include "DescriptorAny.h"
#include "StringRef.h"

#include <ViewDesign/view/frame/ReferenceFrame.h>


class ItemBlock : private item_block_ref, private ItemRef, public ViewFrame {
private:
	ItemBlock(item_block_ref ref, auto init) : item_block_ref(std::move(ref)), ItemRef(Read(std::move(init))), ViewFrame(new View(*this)) {}
public:
	ItemBlock(item_block_ref ref) : ItemBlock(std::move(ref), nullptr) {}
	ItemBlock(item_block_ref ref, StringTable& string_table) : ItemBlock(std::move(ref), [&]() {
		return std::make_unique<DescriptorAny>(
			std::make_unique<ItemDescriptor>(
				std::make_unique<StringRef>(string_table.Insert(u""))
			)
		);
	}) {}

private:
	std::unique_ptr<Item> Read(auto init) const {
		if (auto data = item_block_ref::read(); data.empty()) {
			if (init == nullptr) {
				throw std::logic_error("ItemBlock: data uninitialized");
			}
			return init();
		} else {
			DeserializeContext context(get_manager(), std::move(data));
			return Item::Construct(DescriptorAny::type, context);
		}
	}

public:
	void SetRoot(ItemRef root) {
		ItemRef::operator=(root);
	}

private:
	class View : public Item::View {
	public:
		View(ItemBlock& item_block) : Item::View(
			new ReferenceFrame(
				item_block.ItemRef::Get().GetView()
			)
		), item_block(item_block), history_context(*this) {}
	private:
		ItemBlock& item_block;
		Context<HistoryContext> history_context;
	private:
		virtual void OnChildUpdate(const View& child, std::unique_ptr<const Item> item) const {
			history_context.Get().OnItemBlockUpdate(item_block, item_block);
			item_block.SetRoot(std::move(item));
		}
	};
};
