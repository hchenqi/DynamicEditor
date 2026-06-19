#pragma once

#include "Descriptor.h"


class DescriptorAny : public Item {
public:
	DescriptorAny(block_view<DescriptorType, DescriptorRegistry::DescriptorCache> descriptor, std::unique_ptr<Descriptor> child) : descriptor(std::move(descriptor)), child(std::move(child)) {}
	DescriptorAny(DeserializeContext& context) : descriptor(context.GetMeta().GetDescriptorRegistry().LookUp(context.access<descriptor_ref>())), child(Descriptor::Construct(descriptor.get(), context)) {}

public:
	static const Type type;
public:
	virtual Type GetType() const override { return type; }

private:
	block_view<DescriptorType, DescriptorRegistry::DescriptorCache> descriptor;
	std::unique_ptr<Descriptor> child;
private:
	virtual void Serialize(SerializeContext& context) const override {
		context.access(descriptor);
		child->Serialize(context);
	}

private:
	virtual std::unique_ptr<View> CreateView() const override { return std::make_unique<View>(child->GetView()); }

private:
	class View : public Item::View {
	public:
		View(Descriptor::View& view) : Item::View(
			new ReferenceFrame(
				view
			)
		) {}
	};
};
