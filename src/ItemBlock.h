#pragma once

#include "item_block_ref.h"
#include "Item.h"
#include "DescriptorAny.h"
#include "StringRef.h"

#include <ViewDesign/view/frame/MutableFrame.h>
#include <ViewDesign/view/control/Placeholder.h>


class ItemBlock : public MutableFrame<Relative, Relative> {
public:
	ItemBlock(item_block_ref ref, Meta& meta) : MutableFrame(new Placeholder<Fixed, Fixed>()), ref(std::move(ref)), root(Deserialize(meta)) {
		Reset(new View(*this));
	}

private:
	item_block_ref ref;
	bool modified = false;
private:
	std::unique_ptr<const Item> Deserialize(Meta& meta) const {
		if (auto data = ref.read(); data.empty()) {
			return std::make_unique<DescriptorAny>(
				std::make_unique<ItemDescriptor>(
					meta.GetDescriptorRegistry(), std::make_unique<StringRef>(meta.GetStringTable().Insert(u""))
				)
			);
		} else {
			Item::DeserializeContext context(ref.get_manager(), std::move(data), meta);
			return Item::Construct(DescriptorAny::type, context);
		}
	}
	void Serialize() {
		SerializeContext context(ref.get_manager());
		root->Serialize(context);
		auto [data, ref_list] = context.Get();
		if (data.size() > block_size_limit) {
			throw std::logic_error("block size exceeds limit");
		}
		ref.write(data, ref_list);
	}

private:
	ItemPtr root;
public:
	ItemPtr SetRoot(ItemPtr root) {
		std::swap(this->root, root);
		Serialize();
		MutableFrame::Reset(new View(*this));
		return root;
	}

private:
	class View : public Item::View {
	public:
		View(ItemBlock& item_block) : Item::View(
			new Item::ViewRef(*this, item_block.root->GetView())
		), item_block(item_block) {}
	private:
		ItemBlock& item_block;
	private:
		virtual void OnChildUpdate(const Item::View& child, std::unique_ptr<const Item> item) const override {
			ItemBlock& item_block = this->item_block;
			GetHistory().OnItemBlockUpdate(item_block.ref, item_block.SetRoot(std::move(item)));
		}
	};
};
