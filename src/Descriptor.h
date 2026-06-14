#pragma once

#include "Item.h"

#include <BlockStore/data/block.h>
#include <CppSerialize/stl/variant.h>


struct DescriptorType;

using DescriptorRef = block<DescriptorType>;


class DescriptorRegistry {
public:

};


class Descriptor : public Item {
public:
	Descriptor() {}


};


class DescriptorAny : public Item {
public:
	static const Type type;
public:
	virtual Type GetType() const override { return type; }

};


class ItemDescriptor : public Descriptor {
public:
	using Type = Item::Type;
public:
	ItemDescriptor(std::unique_ptr<Item> item)
	ItemDescriptor(Type type, DeserializeContext& context) : type(type), {}

private:
	Type type;
public:
	virtual DescriptorType GetType() const override { return RegisterDescriptor(descriptor); }

private:
	ItemRef item;

};


class TupleDescriptor : public Descriptor {
public:
	using Type = std::vector<DescriptorRef>;

};


class DynamicLengthArrayDescriptor : public Descriptor {

};


struct DescriptorType : std::variant<ItemDescriptor::Type, TupleDescriptor::Type, DynamicLengthArrayDescriptor::Type> {
	using layout_base = std::variant<ItemDescriptor::Type, TupleDescriptor::Type, DynamicLengthArrayDescriptor::Type>;
};
