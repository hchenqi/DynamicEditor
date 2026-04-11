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
		width.normal(800px);
		height.normal(500px);
		title.text.assign(L"DynamicEditor");
	}
};


class Editor : public EditBox {
public:
	Editor(block<std::wstring> ref) : EditBox(Style{}, ref.read([] { return L"Type here..."; })), ref(std::move(ref)) {}
private:
	block<std::wstring> ref;
private:
	virtual void OnTextUpdate() override { EditBox::OnTextUpdate(); ref.write(text); }
};


int main() {
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
	return 0;
}
