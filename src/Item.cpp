#include "Item.h"

#include "ItemBlockRef.h"
#include "StringRef.h"
#include "DescriptorAny.h"


namespace {

std::vector<std::function<std::unique_ptr<const Item>(Item::DeserializeContext&)>> item_type_registry = {};

} // namespace


Item::Type Item::RegisterType(std::function<std::unique_ptr<const Item>(DeserializeContext&)> constructor) {
	Item::Type type = item_type_registry.size();
	item_type_registry.emplace_back(std::move(constructor));
	return type;
}

std::unique_ptr<const Item> Item::Construct(const Type& type, DeserializeContext& context) {
	if (type >= item_type_registry.size()) {
		throw std::logic_error("Item: type not registered");
	}
	return item_type_registry[type](context);
}


const Item::Type ItemBlockRef::type = RegisterType<ItemBlockRef>();
const Item::Type StringRef::type = RegisterType<StringRef>();
const Item::Type DescriptorAny::type = RegisterType<DescriptorAny>();
