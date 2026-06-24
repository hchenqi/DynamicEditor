#pragma once

#include "Item.h"

#include <ViewDesign/view/frame/FixedFrame.h>
#include <ViewDesign/view/frame/InnerBorderFrame.h>
#include <ViewDesign/view/frame/MinFrame.h>
#include <ViewDesign/view/layout/SplitLayout.h>
#include <ViewDesign/view/layout/DivideLayout.h>
#include <ViewDesign/view/layout/ListLayout.h>
#include <ViewDesign/view/widget/FilledButton.h>

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
		View(const Descriptor& item, child_type child);
	protected:
		Type RegisterDescriptor(auto type) const { return GetMeta().GetDescriptorRegistry().Insert(DescriptorType(std::move(type))); }
	private:
		class ConvertButton : public FilledButton<Fixed, Fixed> {
		public:
			ConvertButton(std::function<void()> callback) : FilledButton(Style(), std::move(callback)) {}
		};
	};

public:
	using Ptr = RefOrOwner<Descriptor>;
};


class ItemDescriptor : public Descriptor {
public:
	using Type = ItemDescriptorType;

public:
	ItemDescriptor(Descriptor::Type descriptor, std::unique_ptr<const Item> child) : Descriptor(std::move(descriptor)), child(std::move(child)) {}
	ItemDescriptor(Descriptor::Type descriptor, const Type& type, DeserializeContext& context) : ItemDescriptor(std::move(descriptor), Item::Construct(type, context)) {}

private:
	ItemPtr child;
private:
	virtual void Serialize(SerializeContext& context) const override { child.Get().Serialize(context); }

private:
	virtual std::unique_ptr<Item::View> CreateView() const override { return std::make_unique<View>(*this); }

private:
	class View : public Descriptor::View {
	public:
		View(const ItemDescriptor& item) : Descriptor::View(
			item,
			new InnerBorderFrame(
				Border(1.0f, ColorCode::Green),
				new ViewRef(*this, item.child.Get().GetView())
			)
		) {}
	private:
		virtual void OnChildUpdate(const Item::View& child, std::unique_ptr<const Item> item) const override {
			Update(std::make_unique<ItemDescriptor>(RegisterDescriptor(item->GetType()), std::move(item)));
		}
	};
};


class TupleDescriptor : public Descriptor {
public:
	using Type = TupleDescriptorType;

public:
	TupleDescriptor(Descriptor::Type descriptor, std::vector<Descriptor::Ptr> child_list) : Descriptor(std::move(descriptor)), child_list(std::move(child_list)) {}
	TupleDescriptor(Descriptor::Type descriptor, const Type& type, DeserializeContext& context) : TupleDescriptor(std::move(descriptor), [&]() {
		std::vector<Descriptor::Ptr> child_list; child_list.reserve(type.size());
		for (const auto& child_ref : type) {
			child_list.emplace_back(Construct(context.GetMeta().GetDescriptorRegistry().LookUp(child_ref), context));
		}
		return child_list;
	}()) {}

private:
	std::vector<Descriptor::Ptr> child_list;
private:
	virtual void Serialize(SerializeContext& context) const override { for (auto& descriptor : child_list) { descriptor.Get().Serialize(context); } }

private:
	virtual std::unique_ptr<Item::View> CreateView() const override { return std::make_unique<View>(*this); }

private:
	class View : public Descriptor::View {
	public:
		View(const TupleDescriptor& item) : Descriptor::View(
			item,
			new InnerBorderFrame(
				Border(1.0f, ColorCode::Orange),
				new ListLayoutVertical(
					0.0f,
					[&]() {
						std::vector<view_ptr<Relative, Auto>> list; list.reserve(item.child_list.size());
						for (const auto& descriptor : item.child_list) {
							list.emplace_back(
								new MinFrame<Relative, Auto>(
									length_zero,
									new ViewRef(*this, descriptor.Get().GetView())
								)
							);
						}
						return list;
					}()
				)
			)
		), item(item) {}
	private:
		const TupleDescriptor& item;
	private:
		virtual void OnChildUpdate(const Item::View& child, std::unique_ptr<const Item> child_item) const override {
			Type type; type.reserve(item.child_list.size());
			std::vector<Descriptor::Ptr> child_list; child_list.reserve(item.child_list.size());
			for (const auto& descriptor : item.child_list) {
				if (&descriptor.Get().GetView() == &child) {
					auto child_descriptor = AsDescriptor(std::move(child_item));
					type.emplace_back(child_descriptor->GetDescriptorRef());
					child_list.emplace_back(std::move(child_descriptor));
				} else {
					type.emplace_back(descriptor.Get().GetDescriptorRef());
					child_list.emplace_back(descriptor);
				}
			}
			Update(std::make_unique<TupleDescriptor>(RegisterDescriptor(std::move(type)), std::move(child_list)));
		}
	};
};


class DynamicLengthArrayDescriptor : public Descriptor {
public:
	using Type = DynamicLengthArrayDescriptorType;

public:
	DynamicLengthArrayDescriptor(Descriptor::Type descriptor, Type type, std::vector<Descriptor::Ptr> child_list) : Descriptor(std::move(descriptor)), type(type), child_list(std::move(child_list)) {}
	DynamicLengthArrayDescriptor(Descriptor::Type descriptor, const Type& type, DeserializeContext& context) : DynamicLengthArrayDescriptor(std::move(descriptor), type, [&]() {
		auto descriptor = context.GetMeta().GetDescriptorRegistry().LookUp(type);
		size_t size = context.access<size_t>();
		std::vector<Descriptor::Ptr> child_list; child_list.reserve(size);
		for (size_t i = 0; i < size; ++i) {
			child_list.emplace_back(Construct(descriptor, context));
		}
		return child_list;
	}()) {}

private:
	Type type;
	std::vector<Descriptor::Ptr> child_list;
private:
	virtual void Serialize(SerializeContext& context) const override { context.access<size_t>(child_list.size()); for (auto& descriptor : child_list) { descriptor.Get().Serialize(context); } }

private:
	virtual std::unique_ptr<Item::View> CreateView() const override { return std::make_unique<View>(*this); }

private:
	class View : public Descriptor::View {
	public:
		View(const DynamicLengthArrayDescriptor& item) : Descriptor::View(
			item,
			new InnerBorderFrame(
				Border(1.0f, ColorCode::Yellow),
				new ListLayoutVertical(
					0.0f,
					[&]() {
						std::vector<view_ptr<Relative, Auto>> list; list.reserve(item.child_list.size());
						for (const auto& descriptor : item.child_list) {
							list.emplace_back(
								new MinFrame<Relative, Auto>(
									length_zero,
									new ViewRef(*this, descriptor.Get().GetView())
								)
							);
						}
						return list;
					}()
				)
			)
		), item(item) {}
	private:
		const DynamicLengthArrayDescriptor& item;
	private:
		virtual void OnChildUpdate(const Item::View& child, std::unique_ptr<const Item> child_item) const override {
			auto child_descriptor = AsDescriptor(std::move(child_item));
			if (child_descriptor->GetDescriptorRef() != item.type) {
				throw std::invalid_argument("DynamicLengthArrayDescriptor: child type mismatch");
			}
			std::vector<Descriptor::Ptr> descriptor_list; descriptor_list.reserve(item.child_list.size());
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


inline Descriptor::View::View(const Descriptor& item, child_type child) : Item::View(
	new SplitLayoutVertical(
		new FixedFrame<Fixed, Auto>(
			30.0f,
			new DivideLayoutHorizontal(
				create<ConvertButton>([&] { GetHistory().Operation([&]() { Update(std::make_unique<TupleDescriptor>(RegisterDescriptor(TupleDescriptor::Type({ item.GetDescriptorRef() })), std::vector<Descriptor::Ptr>({ item }))); }); }),
				create<ConvertButton>([&] {}),
				create<ConvertButton>([&] {})
			)
		),
		std::move(child)
	)
) {}
