#pragma once

#include "Item.h"
#include "MainWindow.h"


class ItemBlockRef : public Item {
public:
	ItemBlockRef(item_block_ref ref) : ref(std::move(ref)) {}
	ItemBlockRef(DeserializeContext& context) : ItemBlockRef(context.access<item_block_ref>()) {}

private:
	static const Type type;
public:
	virtual Type GetType() const override { return type; }

private:
	item_block_ref ref;

private:
	virtual std::unique_ptr<Item::View> CreateView() const override {
		return std::make_unique<View>(*this);
	}

public:
	class View : public Item::View, private Context<MainWindow>, private ContextProvider {
	public:
		View(ItemBlockRef& item) : Item::View(
			new OpenItemBlockTabButton()
		), Context(AsViewBase()), ContextProvider(AsViewBase()), item(item) {}

	private:
		ItemBlockRef& item;

	private:
		void OpenItemBlockTab() {
			Context::Get().OpenItemBlockTab(item.ref);
		}

	private:
		class OpenItemBlockTabButton : public Button<Placeholder<Auto, Auto>>, private Context<View> {
		public:
			OpenItemBlockTabButton() : Base(Size(20.0f, 20.0f)), Context(AsViewBase()) {}
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
			virtual void OnClick() override { Context::Get().OpenItemBlockTab(); }
		};
	};
};
