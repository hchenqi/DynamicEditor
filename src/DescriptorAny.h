#pragma once

#include "Descriptor.h"


class DescriptorAny : public Item {
public:
	DescriptorAny(std::unique_ptr<const Descriptor> child) : child(std::move(child)) {}
	DescriptorAny(DeserializeContext& context) : DescriptorAny(Descriptor::Construct(context.GetMeta().GetDescriptorRegistry().LookUp(context.access<descriptor_ref>()), context)) {}

public:
	static const Type type;
public:
	virtual Type GetType() const override { return type; }

private:
	std::unique_ptr<const Descriptor> child;
private:
	virtual void Serialize(SerializeContext& context) const override {
		context.access(child->GetDescriptorRef());
		child->Serialize(context);
	}

private:
	virtual std::unique_ptr<View> CreateView() const override { return std::make_unique<View>(child->GetView()); }

private:
	class View : public Item::View {
	public:
		View(Item::View& view) : Item::View(
			new ViewRef(*this, view)
		) {}
	private:
		virtual void OnChildUpdate(const Item::View& child, std::unique_ptr<const Item> item) const override {
			Update(std::make_unique<DescriptorAny>(Descriptor::AsDescriptor(std::move(item))));
		}
	};
};
