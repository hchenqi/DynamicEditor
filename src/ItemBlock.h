#include "Item.h"


class ItemBlockCache {

};





class ItemBlockRef : public Item {	
public:
	static const Type type;
public:
	virtual Type GetType() const override { return type; }


public:
	class View : public Item::View {

	};


};
