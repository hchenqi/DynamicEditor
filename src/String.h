#include "Item.h"

#include <BlockStore/item/OrderedRefSet.h>

#include <ViewDesign/view/layout/SplitLayout.h>
#include <ViewDesign/view/control/TextView.h>
#include <ViewDesign/view/control/TextEditor.h>


template<class T>
struct CacheType {
	using Type = BlockCacheDynamicAdapter<T>;
};

template<>
struct CacheType<std::u16string> {
	using Type = BlockCache<std::u16string>;
};

template<class T>
using Cache = CacheType<T>::Type;


using StringTable = OrderedRefSet<std::u16string, Cache>;


class StringRef : public Item {
public:
	static const Type type;
public:
	virtual Type GetType() const override { return type; }

private:
	static StringTable& GetStringTable() {}

public:
	StringRef(std::u16string str) : str(GetStringTable().insert(std::move(str))) {}
	StringRef(DeserializeContext& context) : str(GetStringTable().insert(context.access<block<std::u16string>>())) {}

private:
	block_view<std::u16string, StringTable::KeyCache> str;

public:
	class View : public Item::View {
	public:
		View(const std::u16string& str) : Item::View(
			new SplitLayoutVertical(
				new TextView(TextView::Style(), str),
				new Editor(*this, str)
			)
		) {}
	private:
		class Editor : public TextEditor {
		public:
			Editor(const View& view, const std::u16string& str) : TextEditor(TextEditor::Style(), str), view(view) {}
		private:
			const View& view;
		private:
			virtual void OnTextUpdate() override {
				TextEditor::OnTextUpdate();
				view.OnTextUpdate(text);
			}
		};
	private:
		void OnTextUpdate(const std::u16string& str) const {
			
			GetHistory().Operation([&]() {
				UpdateSelf(std::make_unique<StringRef>(str));
			});
		}
	};
private:
	virtual std::unique_ptr<Item::View> CreateView() const override {
		return std::make_unique<View>(str.get());
	}
};
