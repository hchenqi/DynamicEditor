#pragma once

#include "Item.h"
#include "DescriptorRegistry.h"
#include "MainWindow.h"

#include <BlockStore/data/block.h>


class Descriptor : public Item {
public:
	using Type = DescriptorType;
public:
	static std::unique_ptr<Descriptor> Construct(const Type& descriptor, DeserializeContext& context);

public:
	class View : public Item::View {
	protected:
		View(child_type child) : Item::View(std::move(child)), meta_context(*this) {}
	private:
		Context<MetaContext> meta_context;
	private:
		DescriptorRegistry& GetDescriptorRegistry() const { return meta_context.Get().GetDescriptorRegistry(); }
	protected:
		block_view<DescriptorType, DescriptorRegistry::DescriptorCache> LookUpDescriptor(descriptor_ref ref) const { return GetDescriptorRegistry().LookUp(std::move(ref)); }
		descriptor_ref RegisterDescriptor(auto descriptor) const { return GetDescriptorRegistry().Insert(DescriptorType(std::move(descriptor))); }
	public:
		virtual descriptor_ref GetDescriptorRef() const = 0;
	};
public:
	View& GetView() const { return static_cast<View&>(Item::GetView()); }
};


class ItemDescriptor : public Descriptor {
public:
	using Type = ItemDescriptorType;

public:
	ItemDescriptor(std::unique_ptr<Item> item) : item(std::move(item)) {}
	ItemDescriptor(Type type, DeserializeContext& context) : item(Item::Construct(type, context)) {}

private:
	ItemRef item;
private:
	virtual void Serialize(SerializeContext& context) const override { item.Get().Serialize(context); }

private:
	virtual std::unique_ptr<Item::View> CreateView() const override { return std::make_unique<View>(); }

private:
	class View : public Descriptor::View {
	private:
		Type descriptor;
	public:
		virtual descriptor_ref GetDescriptorRef() const override { return RegisterDescriptor(descriptor); }
	};
};


class TupleDescriptor : public Descriptor {
public:
	using Type = TupleDescriptorType;

public:
	TupleDescriptor(std::vector<std::unique_ptr<Descriptor>> child_list) : child_list(std::move(child_list)) {}
	TupleDescriptor(Type type, DeserializeContext& context) {
		child_list.reserve(type.size());
		for (const auto& child_type : type) {
			child_list.emplace_back(Construct(context.GetMeta().GetDescriptorRegistry().LookUp(child_type).get(), context));
		}
	}

private:
	std::vector<std::unique_ptr<Descriptor>> child_list;

private:
	virtual std::unique_ptr<Item::View> CreateView() const override { return std::make_unique<View>(); }

private:
	class View : public Descriptor::View {
	private:
		Type descriptor;
	public:
		virtual descriptor_ref GetDescriptorRef() const override { return RegisterDescriptor(descriptor); }
	};
};


class DynamicLengthArrayDescriptor : public Descriptor {
public:
	using Type = DynamicLengthArrayDescriptorType;

private:
	virtual std::unique_ptr<Item::View> CreateView() const override { return std::make_unique<View>(); }

private:
	class View : public Descriptor::View {
	private:
		Type descriptor;
	public:
		virtual descriptor_ref GetDescriptorRef() const override { return RegisterDescriptor(descriptor); }
	};
};


using DescriptorMap = TypeMap<
	TypeMapEntry<ItemDescriptorType, ItemDescriptor>,
	TypeMapEntry<TupleDescriptorType, TupleDescriptor>,
	TypeMapEntry<DynamicLengthArrayDescriptorType, DynamicLengthArrayDescriptor>
>;

inline std::unique_ptr<Descriptor> Descriptor::Construct(const Type& descriptor, DeserializeContext& context) {
	return std::visit([&](const auto& descriptor) -> std::unique_ptr<Descriptor> { return std::make_unique<MappedType<DescriptorMap, std::remove_cvref_t<decltype(descriptor)>>>(std::move(descriptor), context); }, descriptor);
}
