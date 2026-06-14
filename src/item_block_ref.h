#pragma once

#include <BlockStore/data/serializer.h>


using namespace BlockStore;


class item_block_ref : public block_ref {
public:
	using layout_base = block_ref;
public:
	using block_ref::block_ref;
	item_block_ref(block_ref&& other) : block_ref(std::move(other)) {}
	item_block_ref(const block_ref& other) : block_ref(other) {}
public:
	item_block_ref& operator=(block_ref&& other) { block_ref::operator=(std::move(other)); return *this; }
	item_block_ref& operator=(const block_ref& other) { block_ref::operator=(other); return *this; }
};
