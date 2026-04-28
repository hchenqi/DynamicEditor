#include "ViewDesign/view/Desktop.h"
#include "ViewDesign/view/control/EditBox.h"
#include "ViewDesign/view/widget/TitleBarWindow.h"

#include "BlockStore/core/manager.h"
#include "BlockStore/data/block.h"
#include "CppSerialize/stl/string.h"


using namespace ViewDesign;
using namespace BlockStore;


struct MainWindowStyle : TitleBarWindow::Style {
	MainWindowStyle() {
		title.text.assign(u"DynamicEditor");
	}
};


class Editor : public EditBox {
public:
	Editor(block<u16string> ref) : EditBox(Style{}, ref.read([] { return u"Type here..."; })), ref(std::move(ref)) {}
private:
	block<u16string> ref;
private:
	virtual void OnTextUpdate() override { EditBox::OnTextUpdate(); ref.write(text); }
};


void App() {
	BlockManager manager("data.db");
	desktop.AddWindow(
		new TitleBarWindow(
			MainWindowStyle(),
			new ClipFrame<Fixed, Fixed, TopLeft>(
				new Editor(manager.get_root())
			)
		)
	);
	desktop.EventLoop();
}
