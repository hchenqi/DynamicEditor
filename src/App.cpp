#include "MainWindow.h"

#include <BlockStore/core/manager.h>


struct Metadata {
	block_ref descriptor_registry;
	block_ref string_table;
	item_block_ref root_item_block;

	friend constexpr auto layout(layout_type<Metadata>) { return declare(&Metadata::descriptor_registry, &Metadata::string_table, &Metadata::root_item_block); }
};


void App() {
	BlockManager manager("data.db");
	Metadata metadata = block<Metadata>(manager.get_root()).read([&]() {
		return Metadata{ manager.allocate(), manager.allocate(), manager.allocate() };
	});
	desktop.AddWindow(new MainWindow(metadata.root_item_block));
	desktop.EventLoop();
}
