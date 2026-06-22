#pragma once

#include "Item.h"
#include "item_block_ref.h"
#include "MainWindow.h"

#include <ViewDesign/view/control/Placeholder.h>
#include <ViewDesign/view/wrapper/Button.h>


class ItemBlockRef : public Item {
public:
	ItemBlockRef(item_block_ref ref) : ref(std::move(ref)) {}
	ItemBlockRef(DeserializeContext& context) : ItemBlockRef(context.access<item_block_ref>()) {}

public:
	static const Type type;
public:
	virtual Type GetType() const override { return type; }

private:
	item_block_ref ref;
private:
	virtual void Serialize(SerializeContext& context) const override { context.access(ref); }

private:
	virtual std::unique_ptr<Item::View> CreateView() const override { return std::make_unique<View>(ref); }

private:
	class View : public Item::View {
	public:
		View(item_block_ref ref) : Item::View(
			new OpenItemBlockTabButton(*this)
		), ref(std::move(ref)) {}

	private:
		item_block_ref ref;

	private:
		void OpenItemBlockTab() {
			GetMainWindow().OpenItemBlockTab(ref);
		}

	private:
		class OpenItemBlockTabButton : public Button<Placeholder<Auto, Auto>> {
		public:
			OpenItemBlockTabButton(View& view) : Base(Size(20.0f, 20.0f)), view(view) {}
		private:
			View& view;
		private:
			static constexpr Color background_normal = color_transparent;
			static constexpr Color background_hovered = ColorCode::Gray;
			static constexpr Color background_pressed = ColorCode::DimGray;
		private:
			Color background = background_normal;
		private:
			void SetBackground(Color background) {
				if (this->background != background) {
					this->background = background;
					Redraw(rect_infinite);
				}
			}
		private:
			virtual void OnDraw(Canvas& canvas, Rect draw_region) override {
				canvas.draw(point_zero, new Rectangle(size, background));
			}
		private:
			virtual void OnHover() override { SetBackground(background_hovered); }
			virtual void OnPress() override { SetBackground(background_pressed); }
			virtual void OnLeave() override { SetBackground(background_normal); }
		private:
			virtual void OnClick() override { view.OpenItemBlockTab(); }
		};
	};
};
