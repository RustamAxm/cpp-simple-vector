#pragma once

#include <algorithm>
#include <cassert>
#include <cstdlib>

template <typename Type>
class ArrayPtr {
public:
    ArrayPtr() = default;

    explicit ArrayPtr(size_t size) {
        if (size == 0){
            raw_ptr_ = nullptr;
        }else{
            raw_ptr_ = new Type [size]();
        }
    }

    explicit ArrayPtr(Type* raw_ptr) noexcept {
        raw_ptr_ = raw_ptr;
    }

    ArrayPtr(const ArrayPtr&) = delete;

    ArrayPtr(ArrayPtr&& other) {
        if (raw_ptr_ != nullptr) {
            delete[] raw_ptr_;
        }
        raw_ptr_ = std::exchange(other.raw_ptr_, nullptr);
    }

    ~ArrayPtr() {
        delete[] raw_ptr_;
    }

    ArrayPtr& operator=(const ArrayPtr&) = delete;

    ArrayPtr& operator=(ArrayPtr&& rhs) {
        if (raw_ptr_ != nullptr) {
            delete[] raw_ptr_;
        }
        raw_ptr_ = std::exchange(rhs.raw_ptr_, nullptr);
        return *this;
    }

    [[nodiscard]] Type* Release() noexcept {
        auto tmp = raw_ptr_;
        raw_ptr_ = nullptr;
        return tmp;
    }

    Type& operator[](size_t index) noexcept {
        return *(raw_ptr_ + index);
    }

    const Type& operator[](size_t index) const noexcept {
        return *(raw_ptr_ + index);
    }

    explicit operator bool() const {
        return raw_ptr_ != nullptr;
    }

    Type* Get() const noexcept {
        return raw_ptr_;
    }

    void swap(ArrayPtr& other) noexcept {
        auto tmp = raw_ptr_;
        raw_ptr_ = other.raw_ptr_;
        other.raw_ptr_ = tmp;
    }

private:
    Type* raw_ptr_ = nullptr;
};
