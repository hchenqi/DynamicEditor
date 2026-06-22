#pragma once

#include "Item.h"
#include "StringTable.h"
#include "Meta.h"
#include "History.h"

#include <ViewDesign/view/layout/SplitLayout.h>
#include <ViewDesign/view/control/TextView.h>
#include <ViewDesign/view/control/TextEditor.h>
#include <ViewDesign/event/timer.h>


class StringRef : public Item {
public:
	StringRef(block_view<std::u16string, StringTable::StringCache> str) : str(std::move(str)) {}
	StringRef(DeserializeContext& context) : StringRef(context.GetMeta().GetStringTable().LookUp(context.access<block<std::u16string>>())) {}

public:
	static const Type type;
public:
	virtual Type GetType() const override { return type; }

private:
	block_view<std::u16string, StringTable::StringCache> str;
private:
	virtual void Serialize(SerializeContext& context) const override { context.access(str); }

private:
	virtual std::unique_ptr<Item::View> CreateView() const override { return std::make_unique<View>(str.get()); }

private:
	class View : public Item::View {
	public:
		View(const std::u16string& str) : Item::View(
			new SplitLayoutVertical(
				new TextView(TextView::Style(), str),
				editor = new Editor(*this, str)
			)
		), meta_context(*this), history_context(*this) {}

	private:
		Context<MetaContext> meta_context;
		Context<HistoryContext> history_context;
	private:
		StringTable& GetStringTable() { return meta_context.Get().GetStringTable(); }

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
			update_timeout.Stop();
			history_context.Get().Operation([&]() {
				Update(std::make_unique<StringRef>(GetStringTable().Insert(editor->GetText())));
			});
		});
	private:
		void OnTextUpdate() const {
			update_timeout.Set(5000);
		}
	};
};
