#pragma once

#include <cstddef>
#include <memory>
#include <utility>

template <typename T, typename Deleter = std::default_delete<T>>
class UniquePtr {
public:
    // ======================== Constructors ========================

    UniquePtr();
    explicit UniquePtr(T* ptr);
    UniquePtr(T* ptr, const Deleter& deleter);
    UniquePtr(T* ptr, Deleter&& deleter);

    // =================== No copy ==================================

    UniquePtr(const UniquePtr&)            = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    // =================== Move ====================================

    UniquePtr(UniquePtr&& other);
    UniquePtr& operator=(UniquePtr&& other);

    // =================== Destructor ==============================

    ~UniquePtr();

    // =================== Observers ===============================

    T*       get();
    const T* get() const;

    explicit operator bool() const;

    T&       operator*();
    const T& operator*() const;

    T*       operator->();
    const T* operator->() const;

    Deleter&       get_deleter();
    const Deleter& get_deleter() const;

    // =================== Modifiers ===============================

    T*   release();
    void reset(T* ptr = nullptr);
    void swap(UniquePtr& other);

private:
    T*      ptr_ = nullptr;
    [[no_unique_address]] Deleter deleter_;
};

// =====================================================================
//  Partial specialization for arrays: UniquePtr<T[]>
// =====================================================================

template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> {
public:
    // ======================== Constructors ========================

    UniquePtr();
    explicit UniquePtr(T* ptr);
    UniquePtr(T* ptr, const Deleter& deleter);
    UniquePtr(T* ptr, Deleter&& deleter);

    // =================== No copy ==================================

    UniquePtr(const UniquePtr&)            = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    // =================== Move ====================================

    UniquePtr(UniquePtr&& other);
    UniquePtr& operator=(UniquePtr&& other);

    // =================== Destructor ==============================

    ~UniquePtr();

    // =================== Observers ===============================

    T*       get();
    const T* get() const;

    explicit operator bool() const;

    T&       operator[](size_t index);
    const T& operator[](size_t index) const;

    Deleter&       get_deleter();
    const Deleter& get_deleter() const;

    // =================== Modifiers ===============================

    T*   release();
    void reset(T* ptr = nullptr);
    void swap(UniquePtr& other);

private:
    T*      ptr_ = nullptr;
    [[no_unique_address]] Deleter deleter_;
};

template <typename T, typename... Args>
UniquePtr<T> make_unique(Args&&... args);

template <typename T, typename D>
UniquePtr<T, D>::UniquePtr() : ptr_(nullptr), deleter_() {}

template <typename T, typename D>
UniquePtr<T, D>::UniquePtr(T* ptr) : ptr_(ptr), deleter_() {}

template <typename T, typename D>
UniquePtr<T, D>::UniquePtr(T* ptr, const D& deleter) : ptr_(ptr), deleter_(deleter) {}

template <typename T, typename D>
UniquePtr<T, D>::UniquePtr(T* ptr, D&& deleter) : ptr_(ptr), deleter_(std::move(deleter)) {}

template <typename T, typename D>
UniquePtr<T, D>::UniquePtr(UniquePtr&& other) : ptr_(other.release()), deleter_(std::move(other.deleter_)) {}

template <typename T, typename D>
UniquePtr<T, D>& UniquePtr<T, D>::operator=(UniquePtr&& other) {
    if (this != &other) {
        reset(other.release());
        deleter_ = std::move(other.deleter_);
    }
    return *this;
}

template <typename T, typename D> UniquePtr<T, D>::~UniquePtr() { reset(); }
template <typename T, typename D> T* UniquePtr<T, D>::get() { return ptr_; }
template <typename T, typename D> const T* UniquePtr<T, D>::get() const { return ptr_; }
template <typename T, typename D> UniquePtr<T, D>::operator bool() const { return ptr_ != nullptr; }
template <typename T, typename D> T& UniquePtr<T, D>::operator*() { return *ptr_; }
template <typename T, typename D> const T& UniquePtr<T, D>::operator*() const { return *ptr_; }
template <typename T, typename D> T* UniquePtr<T, D>::operator->() { return ptr_; }
template <typename T, typename D> const T* UniquePtr<T, D>::operator->() const { return ptr_; }
template <typename T, typename D> D& UniquePtr<T, D>::get_deleter() { return deleter_; }
template <typename T, typename D> const D& UniquePtr<T, D>::get_deleter() const { return deleter_; }

template <typename T, typename D>
T* UniquePtr<T, D>::release() {
    T* temp = ptr_;
    ptr_ = nullptr;
    return temp;
}

template <typename T, typename D>
void UniquePtr<T, D>::reset(T* ptr) {
    T* old_ptr = ptr_;
    ptr_ = ptr;
    if (old_ptr) deleter_(old_ptr);
}

template <typename T, typename D>
void UniquePtr<T, D>::swap(UniquePtr& other) {
    std::swap(ptr_, other.ptr_);
    std::swap(deleter_, other.deleter_);
}

template <typename T, typename D> UniquePtr<T[], D>::UniquePtr() : ptr_(nullptr), deleter_() {}
template <typename T, typename D> UniquePtr<T[], D>::UniquePtr(T* p) : ptr_(p), deleter_() {}
template <typename T, typename D> UniquePtr<T[], D>::UniquePtr(T* p, const D& d) : ptr_(p), deleter_(d) {}
template <typename T, typename D> UniquePtr<T[], D>::UniquePtr(T* p, D&& d) : ptr_(p), deleter_(std::move(d)) {}
template <typename T, typename D> UniquePtr<T[], D>::UniquePtr(UniquePtr&& o) : ptr_(o.release()), deleter_(std::move(o.deleter_)) {}
template <typename T, typename D> UniquePtr<T[], D>& UniquePtr<T[], D>::operator=(UniquePtr&& o) { if (this != &o) { reset(o.release()); deleter_ = std::move(o.deleter_); } return *this; }
template <typename T, typename D> UniquePtr<T[], D>::~UniquePtr() { reset(); }
template <typename T, typename D> T* UniquePtr<T[], D>::get() { return ptr_; }
template <typename T, typename D> const T* UniquePtr<T[], D>::get() const { return ptr_; }
template <typename T, typename D> UniquePtr<T[], D>::operator bool() const { return ptr_ != nullptr; }
template <typename T, typename D> T& UniquePtr<T[], D>::operator[](size_t i) { return ptr_[i]; }
template <typename T, typename D> const T& UniquePtr<T[], D>::operator[](size_t i) const { return ptr_[i]; }
template <typename T, typename D> D& UniquePtr<T[], D>::get_deleter() { return deleter_; }
template <typename T, typename D> const D& UniquePtr<T[], D>::get_deleter() const { return deleter_; }
template <typename T, typename D> T* UniquePtr<T[], D>::release() { T* r = ptr_; ptr_ = nullptr; return r; }
template <typename T, typename D> void UniquePtr<T[], D>::reset(T* p) { T* old = ptr_; ptr_ = p; if (old) deleter_(old); }
template <typename T, typename D> void UniquePtr<T[], D>::swap(UniquePtr& o) { std::swap(ptr_, o.ptr_); std::swap(deleter_, o.deleter_); }

template <typename T, typename... Args>
UniquePtr<T> make_unique(Args&&... args) {
    return UniquePtr<T>(new T(std::forward<Args>(args)...));
}
