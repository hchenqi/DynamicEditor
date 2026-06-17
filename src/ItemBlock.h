#pragma once

#include "Item.h"
#include "Descriptor.h"
#include "StringRef.h"

#include <ViewDesign/view/frame/ReferenceFrame.h>


class ItemBlock : private item_block_ref, private ItemRef, public ViewFrame, private Context<MainWindow> {
public:
	ItemBlock(item_block_ref ref) : item_block_ref(std::move(ref)), ItemRef(Read()), ViewFrame() {}
	~ItemBlock() {}

private:
	std::unique_ptr<Item> Read() const {
		if (auto data = item_block_ref::read(); data.empty()) {
			return std::make_unique<DescriptorAny>(
				std::make_unique<ItemDescriptor>(
					std::make_unique<StringRef>(Context::Get().GetStringTable().Insert(u""))
				)
			);
		} else {
			DeserializeContext context(get_manager(), std::move(data));
			return Item::Construct(DescriptorAny::type, context);
		}
	}

public:
	void SetRoot(ItemRef root) {
		this->root = root;
	}

private:
	class View : public Item::View {
	public:
		View(ItemBlock& item_block) : Item::View(
			new ReferenceFrame(
				item_block.ItemRef::Get().GetView()
			)
		), item_block(item_block) {}
	private:
		ItemBlock& item_block;
	private:
		virtual void OnChildUpdate(const View& child, std::unique_ptr<const Item> item) const {
			GetHistory().OnItemBlockUpdate(item_block, item_block);
			item_block.SetRoot(std::move(item));
		}
	};
};
