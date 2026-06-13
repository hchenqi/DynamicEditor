// COWRef (Copy-On-Write Reference)

#include <functional>
#include <memory>

template<class T>
class COWRef {
private:
	std::reference_wrapper<const T> ref;
	std::unique_ptr<const T> ptr;
public:
	COWRef(std::unique_ptr<const T> ptr) : ref(*ptr), ptr(std::move(ptr)) {}
	COWRef(const COWRef& other) : ref(other.ref), ptr(nullptr) {}
	COWRef(COWRef&& other) : ref(other.ref), ptr(std::move(other.ptr)) {}
public:
	void operator=(const COWRef& other) { ref = other.ref; ptr = nullptr; }
	void operator=(COWRef&& other) { ref = other.ref; ptr = std::move(other.ptr); }
public:
	const T& Get() const { return ref.get(); }
	void Set(std::unique_ptr<const T> ptr) { operator=(std::move(ptr)); }
};
