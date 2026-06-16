#pragma once

#include "Item.h"
#include "StringTable.h"
#include "MainWindow.h"

#include <ViewDesign/view/layout/SplitLayout.h>
#include <ViewDesign/view/control/TextView.h>
#include <ViewDesign/view/control/TextEditor.h>
#include <ViewDesign/event/timer.h>


class StringRef : public Item {
public:
	static const Type type;
public:
	virtual Type GetType() const override { return type; }

public:
	StringRef(block<std::u16string> ref) : ref(std::move(ref)) {}
	StringRef(DeserializeContext& context) : StringRef(context.access<block<std::u16string>>()) {}

private:
	block<std::u16string> ref;

public:
	class View : public Item::View, private Context<MainWindow> {
	public:
		View(const std::u16string& str) : Item::View(
			new SplitLayoutVertical(
				new TextView(TextView::Style(), str),
				editor = new Editor(*this, str)
			)
		), Context(AsViewBase()) {}

	private:
		StringTable& GetStringTable() { return Context::Get().GetStringTable(); }

	private:
		class Editor : public TextEditor {
		public:
			Editor(const View& view, const std::u16string& str) : TextEditor(TextEditor::Style(), str), view(view) {}
		private:
			const View& view;
		private:
			virtual void OnTextUpdate() override {
				TextEditor::OnTextUpdate();
				view.OnTextUpdate();
			}
		};

	private:
		ref_ptr<Editor> editor;
	private:
		mutable Timer update_timeout = Timer([&]() {
			GetHistory().Operation([&]() {
				UpdateSelf(std::make_unique<StringRef>(editor->GetText()));
			});
		});
	private:
		void OnTextUpdate() const {
			update_timeout.Set(5000);
		}
	};
private:
	virtual std::unique_ptr<Item::View> CreateView() const override {
		return std::make_unique<View>(str.get());
	}
};
