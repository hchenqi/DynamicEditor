#pragma once

#include "COWRef.h"

#include <BlockStore/data/serializer.h>

#include <ViewDesign/view/frame/ViewFrame.h>

#include <functional>


using namespace BlockStore;
using namespace ViewDesign;


class Item {
public:
	using Type = size_t;
private:
	static Type RegisterType(std::function<std::unique_ptr<Item>(DeserializeContext&)> constructor);
public:
	static std::unique_ptr<Item> Construct(Type type, DeserializeContext& context);
protected:
	template<class Derived>
	static Type RegisterType() {
		return RegisterType([](DeserializeContext& context) { return std::make_unique<Derived>(context); });
	}
public:
	virtual Type GetType() const { return -1; }

public:
	class View : public ViewFrame {
	protected:
		View(view_ptr_any child) : ViewFrame(std::move(child)) {}

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
