#pragma once

#include "ViewDesign/view/layout/SplitLayout.h"
#include "ViewDesign/view/layout/ListLayout.h"
#include "ViewDesign/view/layout/StackLayout.h"
#include "ViewDesign/view/frame/ScrollFrame.h"
#include "ViewDesign/view/frame/FixedFrame.h"
#include "ViewDesign/view/frame/PaddingFrame.h"
#include "ViewDesign/view/frame/CenterFrame.h"
#include "ViewDesign/view/frame/MaxFrame.h"
#include "ViewDesign/view/frame/ClipFrame.h"
#include "ViewDesign/view/frame/MutableFrame.h"
#include "ViewDesign/view/control/Placeholder.h"
#include "ViewDesign/view/control/TextView.h"
#include "ViewDesign/view/wrapper/Button.h"
#include "ViewDesign/view/wrapper/HitTestHelper.h"
#include "ViewDesign/view/figure/shape.h"
#include "ViewDesign/messaging/context.h"


using namespace ViewDesign;


class TabView : public ViewFrame, public SizeTrait<Fixed, Fixed> {
public:
	struct Tab {
		view_ptr<Auto, Fixed> header;
		view_ptr<Relative, Relative> content;

		Tab(view_ptr<Auto, Fixed> header, view_ptr<Relative, Relative> content) : header(std::move(header)), content(std::move(content)) {}
	};

	class HeaderFrame : public HitSelfFallbackRecursive<ViewFrame>, public SizeTrait<Auto, Fixed>, private ContextProvider {
	public:
		void Focus() {

		}
		void Close() {

		}

	private:
		virtual void OnMouseEvent(MouseEvent event) override {
			switch (event.type) {
			case MouseEvent::LeftDown: Focus(); break;
			}
		}
	};

	using ContentFrame = MutableFrame<Fixed, Fixed>;

	class FixedHeader : public HitThrough<PaddingFrame<Auto, Fixed>> {
	public:
		FixedHeader(u16string text) : Base(
			Padding(10.0f),
			new CenterFrame<Auto, Fixed>(
				new MaxFrame<Auto, Relative>(
					200.0f,
					new TextView(TextView::Style(), std::move(text))
				)
			)
		) {}
	};

	class DefaultHeader : public StackLayout<Auto, Fixed, Fixed, Fixed> {
	public:
		DefaultHeader(u16string text) : StackLayout(
			new FixedHeader(std::move(text)),
			new HitSelf<CenterFrame<Fixed, Fixed>>(
				new HitThroughMargin<ClipFrame<Fixed, Auto, Right>>(
					new PaddingFrame(
						Padding(10.0f),
						new CloseButton()
					)
				)
			)
		) {}

	private:
		class CloseButton : public Button<Placeholder<Auto, Auto>>, private Context<HeaderFrame> {
		public:
			CloseButton() : Base(Size(10.0f, 10.0f)), Context(AsViewBase()) {}
		private:
			static constexpr Color foreground = ColorCode::White;
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
				canvas.draw(Point(5.0f, 5.0f), new Circle(5.0f, background));
				canvas.draw(point_zero, new Line(Vector(10.0f, 10.0f), 1.0f, foreground));
				canvas.draw(Point(10.0f, 0.0f), new Line(Vector(10.0f, -10.0f), 1.0f, foreground));
			}
		private:
			virtual void OnHover() override { SetBackground(background_hovered); }
			virtual void OnPress() override { SetBackground(background_pressed); }
			virtual void OnLeave() override { SetBackground(background_normal); }
		private:
			virtual void OnClick() override { Context::Get().Close(); }
		};
	};

public:
	TabView() : ViewFrame(
		new SplitLayoutVertical(
			new FixedFrame<Fixed, Auto>(
				30.0f,
				new ScrollFrame<Horizontal>(

				)
			),
			content_frame = new ContentFrame(
				new Placeholder<Fixed, Fixed>()
			)
		)
	) {}

private:
	ref_ptr<ContentFrame> content_frame;

private:
	

private:
	void OnTabFocus() {
		
	}

public:
	void Append(Tab tab) {}
	void Insert() {}
};
