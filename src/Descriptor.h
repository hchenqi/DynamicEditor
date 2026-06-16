#pragma once

#include "Item.h"
#include "DescriptorRegistry.h"
#include "MainWindow.h"

#include <BlockStore/data/block.h>


class Descriptor : public Item {
protected:
	using Type = DescriptorType;

protected:
	class View : public Item::View, private Context<MainWindow> {
	protected:
		std::unique_ptr<Descriptor> ConstructDescriptor(descriptor_ref ref, DeserializeContext& context);
	private:
		DescriptorRegistry& GetDescriptorRegistry() const { return Context::Get().GetDescriptorRegistry(); }
	protected:
		block_view<DescriptorType, DescriptorRegistry::DescriptorCache> LookUpDescriptor(descriptor_ref ref) const { return GetDescriptorRegistry().LookUp(std::move(ref)); }
		descriptor_ref RegisterDescriptor(auto descriptor) const { return GetDescriptorRegistry().Insert(DescriptorType(std::move(descriptor))); }
	public:
		virtual Type GetDescriptorType() const = 0;
	};
};


class DescriptorAny : public Item {
public:
	static const Type type;
public:
	virtual Type GetType() const override { return type; }

public:
	DescriptorAny(DeserializeContext& context) : descriptor_type(context.access<descriptor_ref>()) {}

private:
	descriptor_ref descriptor_type;
};


class ItemDescriptor : public Descriptor {
public:
	using Type = ItemDescriptorType;

public:
	ItemDescriptor(std::unique_ptr<Item> item) {}
	ItemDescriptor(Type type, DeserializeContext& context) : type(type), {}

private:
	ItemRef item;

private:
	class View : public Descriptor::View {
	private:
		Type descriptor;
	public:
		virtual DescriptorType GetDescriptorType() const override { return RegisterDescriptor(descriptor); }
	};
};


class TupleDescriptor : public Descriptor {
public:
	using Type = TupleDescriptorType;

private:
	class View : public Descriptor::View {
	private:
		Type descriptor;
	public:
		virtual DescriptorType GetDescriptorType() const override { return RegisterDescriptor(descriptor); }
	};
};


class DynamicLengthArrayDescriptor : public Descriptor {
public:
	using Type = DynamicLengthArrayDescriptorType;

private:
	class View : public Descriptor::View {
	private:
		Type descriptor;
	public:
		virtual DescriptorType GetDescriptorType() const override { return RegisterDescriptor(descriptor); }
	};
};


using DescriptorMap = TypeMap<
	TypeMapEntry<ItemDescriptorType, ItemDescriptor>,
	TypeMapEntry<TupleDescriptorType, TupleDescriptor>,
	TypeMapEntry<DynamicLengthArrayDescriptorType, DynamicLengthArrayDescriptor>
>;

inline std::unique_ptr<Descriptor> Descriptor::View::ConstructDescriptor(descriptor_ref ref, DeserializeContext& context) {
	return std::visit([&](const auto& descriptor) -> std::unique_ptr<Descriptor> { return std::make_unique<MappedType<DescriptorMap, std::remove_cvref_t<decltype(descriptor)>>>(std::move(descriptor), context); }, LookUpDescriptor(std::move(ref)).get());
}
