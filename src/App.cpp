#include "MainWindow.h"

#include <BlockStore/core/manager.h>


void App() {
	BlockManager manager("data.db");
	desktop.AddWindow(new MainWindow(manager.get_root()));
	desktop.EventLoop();
}
