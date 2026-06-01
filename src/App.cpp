#include "ViewDesign/view/widget/DefaultWindow.h"
#include "ViewDesign/view/frame/ClipFrame.h"
#include "ViewDesign/view/frame/PaddingFrame.h"
#include "ViewDesign/view/control/TextEditor.h"
#include "ViewDesign/view/wrapper/Background.h"

#include "BlockStore/core/manager.h"
#include "BlockStore/data/block.h"
#include "CppSerialize/stl/string.h"


using namespace ViewDesign;
using namespace BlockStore;


class Editor : public TextEditor {
public:
	Editor(block<u16string> ref) : TextEditor(Style{}, ref.read([] { return u"Type here..."; })), ref(std::move(ref)) {}
private:
	block<u16string> ref;
private:
	virtual void OnTextUpdate() override { TextEditor::OnTextUpdate(); ref.write(text); }
};


void App() {
	BlockManager manager("data.db");
	desktop.AddWindow(
		new DefaultBackground<DefaultWindow>(
			DefaultWindow::Style(),
			u"DynamicEditor",
			new ClipFrame<Fixed, Fixed, TopLeft>(
				new PaddingFrame(
					Padding(5.0f),
					new Editor(manager.get_root())
				)
			)
		)
	);
	desktop.EventLoop();
}
