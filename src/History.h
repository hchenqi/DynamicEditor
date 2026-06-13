
#include <vector>


class History {
private:
	class Entry {

	};

public:
	void OnItemBlockUpdate(ItemBlock& item_block, ) {
		
	}

public:
	void BeginOperation() {
		
	}
	void CancelOperation() {
		
	}
	void EndOperation() {

	}
public:
	void Operation(auto func) {
		BeginOperation();
		func();
		EndOperation();
	}
};
