#pragma once

#include <functional>
#include <memory>


template<class T>
class RefOrOwner {
private:
	std::reference_wrapper<const T> ref;
	std::unique_ptr<const T> ptr;
public:
	RefOrOwner(const T& ref) : ref(ref), ptr(nullptr) {}
	RefOrOwner(std::unique_ptr<const T> ptr) : ref(*ptr), ptr(std::move(ptr)) {}
	RefOrOwner(const RefOrOwner& other) : ref(other.ref), ptr(nullptr) {}
	RefOrOwner(RefOrOwner&& other) : ref(other.ref), ptr(std::move(other.ptr)) {}
public:
	void operator=(const RefOrOwner& other) { ref = other.ref; ptr = nullptr; }
	void operator=(RefOrOwner&& other) { ref = other.ref; ptr = std::move(other.ptr); }
public:
	const T& Get() const { return ref.get(); }
};
