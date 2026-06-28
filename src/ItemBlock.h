#pragma once

#include "item_block_ref.h"
#include "Item.h"
#include "DescriptorAny.h"
#include "StringRef.h"

#include <ViewDesign/view/frame/MutableFrame.h>
#include <ViewDesign/view/control/Placeholder.h>
#include <ViewDesign/common/holder.h>


class ItemBlock : private Holder<item_block_ref>, private Holder<ItemPtr>, public MutableFrame<Relative, Relative> {
public:
	ItemBlock(const item_block_ref& ref, Meta& meta) : Holder<item_block_ref>(ref), Holder<ItemPtr>(Deserialize(meta)), MutableFrame(new View(*this)) {}

private:
	item_block_ref& GetRef() { return Holder<item_block_ref>::value; }
	const item_block_ref& GetRef() const { return Holder<item_block_ref>::value; }
private:
	std::unique_ptr<const Item> Deserialize(Meta& meta) const {
		if (auto data = GetRef().read(); data.empty()) {
			return std::make_unique<DescriptorAny>(
				std::make_unique<ItemDescriptor>(
					meta.GetDescriptorRegistry(), std::make_unique<StringRef>(meta.GetStringTable().Insert(u""))
				)
			);
		} else {
			Item::DeserializeContext context(GetRef().get_manager(), std::move(data), meta);
			return Item::Construct(DescriptorAny::type, context);
		}
	}

private:
	bool modified = false;
private:
	void Serialize() {
		SerializeContext context(GetRef().get_manager());
		GetRoot()->Serialize(context);
		auto [data, ref_list] = context.Get();
		if (data.size() > block_size_limit) {
			throw std::logic_error("block size exceeds limit");
		}
		GetRef().write(data, ref_list);
	}

private:
	ItemPtr& GetRoot() { return Holder<ItemPtr>::value; }
public:
	ItemPtr SetRoot(ItemPtr root) {
		std::swap(GetRoot(), root);
		Serialize();
		MutableFrame::Reset(new View(*this));
		return root;
	}

private:
	class View : public Item::View {
	public:
		View(ItemBlock& item_block) : Item::View(
			new Item::ViewRef(*this, item_block.GetRoot()->GetView())
		), item_block(item_block) {}
	private:
		ItemBlock& item_block;
	private:
		virtual void OnChildUpdate(const Item::View& child, std::unique_ptr<const Item> item) const override {
			ItemBlock& item_block = this->item_block;
			GetHistory().OnItemBlockUpdate(item_block.GetRef(), item_block.SetRoot(std::move(item)));
		}
	};
};
