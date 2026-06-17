#include <ViewDesign/view/view_traits.h>

#include <functional>


using namespace ViewDesign;


class LazyLoadFrame : public ViewBase, public SizeTrait<Relative, Relative> {
public:
	LazyLoadFrame(std::function<view_ptr_any()> load) : load(std::move(load)), child() {}
	virtual ~LazyLoadFrame() override {}

	// child
protected:
	std::function<view_ptr_any()> load;
	view_ptr_any child;
protected:
	void CheckChild() {
		if (child == nullptr) {
			throw std::logic_error("LazyLoadFrame: invalid child view");
		}
	}
	void CheckLoadChild() {
		if (child == nullptr) {
			child = load();
			CheckChild();
			RegisterChild(*child);
		}
	}

	// layout
protected:
	virtual Size OnSizeRefUpdate(Size size_ref) override { CheckLoadChild(); return UpdateChildSizeRef(child, size_ref); }
	virtual void OnChildSizeUpdate(ViewBase& child, Size child_size) override { return SizeUpdated(child_size); }

	// drawing
protected:
	virtual void OnDraw(Canvas& canvas, Rect draw_region) override { CheckChild(); DrawChild(child, point_zero, canvas, draw_region); }
	virtual void OnChildRedraw(ViewBase& child, Rect child_redraw_region) override { Redraw(child_redraw_region); }

	// event
protected:
	virtual ref_ptr<ViewBase> HitTest(MouseEvent& event) override { CheckChild(); return HitTestChild(child, event); }
};
