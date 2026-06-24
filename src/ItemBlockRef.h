#pragma once

#include "Item.h"
#include "item_block_ref.h"
#include "MainWindow.h"

#include <ViewDesign/view/widget/FilledButton.h>


class ItemBlockRef : public Item {
public:
	ItemBlockRef(item_block_ref ref) : ref(std::move(ref)) {}
	ItemBlockRef(DeserializeContext& context) : ItemBlockRef(context.access<item_block_ref>()) {}

public:
	static const Type type;
public:
	virtual Type GetType() const override { return type; }

private:
	item_block_ref ref;
private:
	virtual void Serialize(SerializeContext& context) const override { context.access(ref); }

private:
	virtual std::unique_ptr<Item::View> CreateView() const override { return std::make_unique<View>(ref); }

private:
	class View : public Item::View {
	public:
		View(const item_block_ref& ref) : Item::View(
			new OpenItemBlockTabButton(OpenItemBlockTabButton::Style(), [&] { GetMainWindow().OpenItemBlockTab(ref); }, Size(20.0f, 20.0f))
		) {}
	private:
		using OpenItemBlockTabButton = FilledButton<Auto, Auto>;
	};
};
