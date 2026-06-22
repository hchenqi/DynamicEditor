#pragma once

#include "ItemType.h"
#include "ItemRef.h"
#include "MainWindow.h"

#include <BlockStore/data/serializer.h>

#include <ViewDesign/view/frame/ViewFrame.h>

#include <functional>


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
	static Type RegisterType(std::function<std::unique_ptr<const Item>(DeserializeContext&)> constructor);
protected:
	template<class Derived>
	static Type RegisterType() {
		return RegisterType([](DeserializeContext& context) { return std::make_unique<Derived>(context); });
	}
public:
	static std::unique_ptr<const Item> Construct(const Type& type, DeserializeContext& context);
public:
	virtual Type GetType() const { return item_type_undefined; }

public:
	virtual void Serialize(SerializeContext& context) const = 0;

public:
	class ViewRef;

	class View : public ViewFrame, public SizeTrait<Relative, Relative>, private Context<MainWindow> {
	private:
		friend class ViewRef;
	protected:
		using child_type = view_ptr<Relative, Relative>;
	protected:
		View(child_type child) : ViewFrame(std::move(child)), Context(AsViewBase()) {}
	private:
		ref_ptr<View> parent = nullptr;
	protected:
		MainWindow& GetMainWindow() const { return Context::Get(); }
		Meta& GetMeta() const { return Context::Get().GetMeta(); }
		History& GetHistory() const { return Context::Get().GetHistory(); }
	protected:
		void Update(std::unique_ptr<const Item> item) const { parent->OnChildUpdate(*this, std::move(item)); }
	protected:
		virtual void OnChildUpdate(const View& child, std::unique_ptr<const Item> item) const { throw std::logic_error("Item::View: unhandled OnChildUpdate()"); }
	};

	class ViewRef : public ViewBase, public SizeTrait<Relative, Relative> {
	public:
		ViewRef(View& parent, View& child) : parent(parent), child(child) {}
		virtual ~ViewRef() override { CheckResetChild(); }
	private:
		View& parent;
		View& child;
	private:
		void CheckSetChild() {
			if (child.HasParent()) {
				if (ViewRef& parent = static_cast<ViewRef&>(child.GetParent()); &parent != this) {
					parent.UnregisterChild(child);
					RegisterChild(child);
				}
			} else {
				RegisterChild(child);
			}
			if (child.parent != &parent) {
				child.parent = &parent;
			}
		}
		void CheckResetChild() {
			if (child.HasParent()) {
				if (ViewRef& parent = static_cast<ViewRef&>(child.GetParent()); &parent == this) {
					UnregisterChild(child);
				}
			}
			if (child.parent == &parent) {
				child.parent = nullptr;
			}
		}
	protected:
		virtual Size OnSizeRefUpdate(Size size_ref) override { CheckSetChild(); return UpdateChildSizeRef(child, size_ref); }
		virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override { return SizeUpdated(child_size); }
	protected:
		virtual void OnDraw(Canvas& canvas, Rect draw_region) override { CheckSetChild(); DrawChild(child, point_zero, canvas, draw_region); }
		virtual void OnChildRedraw(ViewBase& child, Rect child_redraw_region) override { Redraw(child_redraw_region); }
	protected:
		virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override { CheckSetChild(); return HitTestChild(child, event); }
	};

private:
	mutable std::unique_ptr<View> view = nullptr;
protected:
	virtual std::unique_ptr<View> CreateView() const = 0;
public:
	View& GetView() const { if (view == nullptr) { view = CreateView(); } return *view; }
};
