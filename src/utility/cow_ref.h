// COWRef (Copy-On-Write Reference) stores either a reference or a `unique_ptr` that is created only when the data needs to be modified

#include <memory>
#include <stdexcept>

template<class T>
class COWRef {
private:
	const T* ref;
	std::unique_ptr<T> ptr;
public:
	COWRef(std::unique_ptr<T> ptr) : ref(ptr.get()), ptr(std::move(ptr)) { if (ref == nullptr) { throw std::invalid_argument("COWRef: ptr is null"); } }
	COWRef(const COWRef& other) : ref(other.ref), ptr() {}
public:
	const T& Read() const { return *ref; }
	T& Write() { if (ptr == nullptr) { ptr = std::make_unique<T>(*ref); ref = ptr.get(); } return *ptr; }
};
