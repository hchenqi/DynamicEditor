#pragma once

#include "Item.h"
#include "DescriptorRegistry.h"

#include <ViewDesign/view/frame/MinFrame.h>
#include <ViewDesign/view/layout/ListLayout.h>

#include <algorithm>


class Descriptor : public Item {
protected:
	using Type = block_view<DescriptorType, DescriptorRegistry::DescriptorCache>;

protected:
	Descriptor(Type descriptor) : descriptor(std::move(descriptor)) {}

private:
	Type descriptor;
public:
	descriptor_ref GetDescriptorRef() const { return descriptor; }
public:
	static std::unique_ptr<const Descriptor> Construct(Type descriptor, DeserializeContext& context);
public:
	static std::unique_ptr<const Descriptor> AsDescriptor(std::unique_ptr<const Item> item) {
		return std::unique_ptr<const Descriptor>(static_cast<const Descriptor*>(item.release()));
	}

public:
	class View : public Item::View {
	protected:
		View(child_type child) : Item::View(std::move(child)), meta_context(*this) {}
	private:
		Context<MetaContext> meta_context;
	protected:
		Type RegisterDescriptor(auto type) const { return meta_context.Get().GetDescriptorRegistry().Insert(DescriptorType(std::move(type))); }
	};

public:
	using Ref = COWRef<Descriptor>;
};


class ItemDescriptor : public Descriptor {
public:
	using Type = ItemDescriptorType;

public:
	ItemDescriptor(Descriptor::Type descriptor, std::unique_ptr<const Item> item) : Descriptor(std::move(descriptor)), item(std::move(item)) {}
	ItemDescriptor(Descriptor::Type descriptor, const Type& type, DeserializeContext& context) : ItemDescriptor(std::move(descriptor), Item::Construct(type, context)) {}

private:
	Item::Ref item;
private:
	virtual void Serialize(SerializeContext& context) const override { item.Get().Serialize(context); }

private:
	virtual std::unique_ptr<Item::View> CreateView() const override { return std::make_unique<View>(item.Get().GetView()); }

private:
	class View : public Descriptor::View {
	public:
		View(Item::View& view) : Descriptor::View(
			new ReferenceFrame(
				view
			)
		) {}
	private:
		virtual void OnChildUpdate(const View& child, std::unique_ptr<const Item> item) const {
			Update(std::make_unique<ItemDescriptor>(RegisterDescriptor(item->GetType()), std::move(item)));
		}
	};
};


class TupleDescriptor : public Descriptor {
public:
	using Type = TupleDescriptorType;

public:
	TupleDescriptor(Descriptor::Type descriptor, std::vector<Descriptor::Ref> child_list) : Descriptor(std::move(descriptor)), child_list(std::move(child_list)) {}
	TupleDescriptor(Descriptor::Type descriptor, const Type& type, DeserializeContext& context) : TupleDescriptor(std::move(descriptor), [&]() {
		std::vector<Descriptor::Ref> child_list; child_list.reserve(type.size());
		for (const auto& child_ref : type) {
			child_list.emplace_back(Construct(context.GetMeta().GetDescriptorRegistry().LookUp(child_ref), context));
		}
		return child_list;
	}()) {}

private:
	std::vector<Descriptor::Ref> child_list;
private:
	virtual void Serialize(SerializeContext& context) const override { for (auto& descriptor : child_list) { descriptor.Get().Serialize(context); } }

private:
	virtual std::unique_ptr<Item::View> CreateView() const override { return std::make_unique<View>(child_list); }

private:
	class View : public Descriptor::View {
	public:
		View(const std::vector<Descriptor::Ref>& descriptor_list) : Descriptor::View(
			new ListLayoutVertical(
				0.0f,
				[&]() {
					std::vector<view_ptr<Relative, Auto>> list; list.reserve(descriptor_list.size());
					for (const auto& descriptor : descriptor_list) {
						list.emplace_back(
							new MinFrame<Relative, Auto>(
								length_zero,
								new ReferenceFrame(descriptor.Get().GetView())
							)
						);
					}
					return list;
				}()
			)
		), descriptor_list(descriptor_list) {}
	private:
		const std::vector<Descriptor::Ref>& descriptor_list;
	private:
		virtual void OnChildUpdate(const View& child, std::unique_ptr<const Item> item) const {
			Type type; type.reserve(this->descriptor_list.size());
			std::vector<Descriptor::Ref> descriptor_list; descriptor_list.reserve(this->descriptor_list.size());
			for (const auto& descriptor : this->descriptor_list) {
				if (&descriptor.Get().GetView() == &child) {
					auto descriptor = AsDescriptor(std::move(item));
					type.emplace_back(descriptor->GetDescriptorRef());
					descriptor_list.emplace_back(std::move(descriptor));
				} else {
					type.emplace_back(descriptor.Get().GetDescriptorRef());
					descriptor_list.emplace_back(descriptor);
				}
			}
			Update(std::make_unique<TupleDescriptor>(RegisterDescriptor(std::move(type)), std::move(descriptor_list)));
		}
	};
};


class DynamicLengthArrayDescriptor : public Descriptor {
public:
	using Type = DynamicLengthArrayDescriptorType;

public:
	DynamicLengthArrayDescriptor(Descriptor::Type descriptor, Type type, std::vector<Descriptor::Ref> child_list) : Descriptor(std::move(descriptor)), type(type), child_list(std::move(child_list)) {}
	DynamicLengthArrayDescriptor(Descriptor::Type descriptor, const Type& type, DeserializeContext& context) : DynamicLengthArrayDescriptor(std::move(descriptor), type, [&]() {
		auto descriptor = context.GetMeta().GetDescriptorRegistry().LookUp(type);
		size_t size = context.access<size_t>();
		std::vector<Descriptor::Ref> child_list; child_list.reserve(size);
		for (size_t i = 0; i < size; ++i) {
			child_list.emplace_back(Construct(descriptor, context));
		}
		return child_list;
	}()) {}

private:
	Type type;
	std::vector<Descriptor::Ref> child_list;
private:
	virtual void Serialize(SerializeContext& context) const override { context.access<size_t>(child_list.size()); for (auto& descriptor : child_list) { descriptor.Get().Serialize(context); } }

private:
	virtual std::unique_ptr<Item::View> CreateView() const override { return std::make_unique<View>(*this); }

private:
	class View : public Descriptor::View {
	public:
		View(const DynamicLengthArrayDescriptor& item) : Descriptor::View(
			new ListLayoutVertical(
				0.0f,
				[&]() {
					std::vector<view_ptr<Relative, Auto>> list; list.reserve(item.child_list.size());
					for (const auto& descriptor : item.child_list) {
						list.emplace_back(
							new MinFrame<Relative, Auto>(
								length_zero,
								new ReferenceFrame(descriptor.Get().GetView())
							)
						);
					}
					return list;
				}()
			)
		), item(item) {}
	private:
		const DynamicLengthArrayDescriptor& item;
	private:
		virtual void OnChildUpdate(const View& child, std::unique_ptr<const Item> child_item) const {
			auto child_descriptor = AsDescriptor(std::move(child_item));
			if (child_descriptor->GetDescriptorRef() != item.type) {
				throw std::invalid_argument("DynamicLengthArrayDescriptor: child type mismatch");
			}
			std::vector<Descriptor::Ref> descriptor_list; descriptor_list.reserve(item.child_list.size());
			for (const auto& descriptor : item.child_list) {
				if (&descriptor.Get().GetView() == &child) {
					descriptor_list.emplace_back(std::move(child_descriptor));
				} else {
					descriptor_list.emplace_back(descriptor);
				}
			}
			Update(std::make_unique<DynamicLengthArrayDescriptor>(RegisterDescriptor(item.type), item.type, std::move(descriptor_list)));
		}
	};
};


using DescriptorMap = TypeMap<
	TypeMapEntry<ItemDescriptorType, ItemDescriptor>,
	TypeMapEntry<TupleDescriptorType, TupleDescriptor>,
	TypeMapEntry<DynamicLengthArrayDescriptorType, DynamicLengthArrayDescriptor>
>;

inline std::unique_ptr<const Descriptor> Descriptor::Construct(Type descriptor, DeserializeContext& context) {
	return std::visit([&](const auto& type) -> std::unique_ptr<const Descriptor> { return std::make_unique<MappedType<DescriptorMap, std::remove_cvref_t<decltype(type)>>>(std::move(descriptor), type, context); }, descriptor.get());
}
