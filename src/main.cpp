#include "WndDesign/window/Global.h"
#include "WndDesign/widget/TitleBarFrame.h"
#include "WndDesign/control/EditBox.h"

#include "BlockStore/core/manager.h"
#include "BlockStore/data/block.h"
#include "CppSerialize/stl/string.h"


using namespace WndDesign;
using namespace BlockStore;


struct MainFrameStyle : TitleBarFrame::Style {
	MainFrameStyle() {
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
	global.AddWnd(
		new TitleBarFrame(
			MainFrameStyle(),
			new ClipFrame<Assigned, Assigned, TopLeft>(
				new Editor(manager.get_root())
			)
		)
	);
	global.MessageLoop();
	return 0;
}
