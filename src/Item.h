#pragma once

#include "Meta.h"
#include "ItemType.h"
#include "COWRef.h"

#include <BlockStore/data/serializer.h>

#include <ViewDesign/view/frame/ViewFrame.h>
#include <ViewDesign/view/frame/ReferenceFrame.h>

#include <functional>


using namespace BlockStore;
using namespace ViewDesign;


class Item {
public:
	class DeserializeContext : public BlockStore::DeserializeContext {
	public:
		DeserializeContext(BlockManager& manager, std::vector<std::byte> data, Meta& meta) : BlockStore::DeserializeContext(manager, std::move(data)), meta(meta) {}
	private:
		Meta& meta;
	public:
		Meta& GetMeta() { return meta; }
	};

public:
	using Type = ItemType;
private:
	static Type RegisterType(std::function<std::unique_ptr<Item>(DeserializeContext&)> constructor);
protected:
	template<class Derived>
	static Type RegisterType() {
		return RegisterType([](DeserializeContext& context) { return std::make_unique<Derived>(context); });
	}
public:
	static std::unique_ptr<Item> Construct(const Type& type, DeserializeContext& context);
public:
	virtual Type GetType() const { return item_type_undefined; }

public:
	virtual void Serialize(SerializeContext& context) const = 0;

public:
	class View : public ViewFrame, public SizeTrait<Relative, Relative> {
	protected:
		using child_type = view_ptr<Relative, Relative>;

	protected:
		View(child_type child) : ViewFrame(std::move(child)) {}

	private:
		ref_ptr<View> parent = nullptr;
	protected:
		void SetChild(View& view) { view.parent = this; }
		void ResetChild(View& view) { view.parent = nullptr; }

	protected:
		void UpdateSelf(std::unique_ptr<const Item> item) const { parent->OnChildUpdate(*this, std::move(item)); }
	protected:
		virtual void OnChildUpdate(const View& child, std::unique_ptr<const Item> item) const {}
	};

private:
	mutable std::unique_ptr<View> view = nullptr;
protected:
	virtual std::unique_ptr<View> CreateView() const = 0;
public:
	View& GetView() const { if (view == nullptr) { view = CreateView(); } return *view; }
};


using ItemRef = COWRef<Item>;
