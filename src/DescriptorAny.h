#pragma once

#include "Descriptor.h"


class DescriptorAny : public Item {
public:
	DescriptorAny(std::unique_ptr<Descriptor> descriptor) : ref(descriptor->GetView().GetDescriptorRef()), descriptor(std::move(descriptor)) {}
	DescriptorAny(DeserializeContext& context) : ref(context.access<descriptor_ref>()) {}

public:
	static const Type type;
public:
	virtual Type GetType() const override { return type; }

private:
	descriptor_ref ref;
	std::unique_ptr<Descriptor> descriptor;

private:
	virtual std::unique_ptr<View> CreateView() const override { return std::make_unique<View>(*this); }

private:
	class View : public Item::View {
	public:
		View(DescriptorAny& item) : Item::View(
			new ReferenceFrame(
				item.descriptor->GetView()
			)
		) {}
	};
};
