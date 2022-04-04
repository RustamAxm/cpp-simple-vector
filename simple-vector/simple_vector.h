#pragma once

#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <iterator>
#include <stdexcept>

#include "array_ptr.h"

struct ReserveProxyObj {
    size_t capacity_to_reserve_;
    ReserveProxyObj(size_t capacity_to_reserve){
        capacity_to_reserve_ = capacity_to_reserve;
    }
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    explicit SimpleVector(size_t size) : vector_(size),
                                         size_(size),
                                         capacity_(size){
    }

    SimpleVector(size_t size, const Type& value) : SimpleVector(size){
        std::fill(begin(),end(), value);
    }

    SimpleVector(std::initializer_list<Type> init) : SimpleVector(init.size()) {
        std::copy(std::make_move_iterator(init.begin()),
                  std::make_move_iterator(init.end()),
                  begin());
    }

    SimpleVector(const SimpleVector& other) {
        SimpleVector<Type> temp(other.size_);
        std::copy(std::make_move_iterator(other.begin()),
                  std::make_move_iterator(other.end()),
                  temp.begin());
        swap(temp);
    }

    SimpleVector(SimpleVector&& other) {
        vector_ = std::move(other.vector_);
        size_ = std::exchange(other.size_, size_);
        capacity_ = std::exchange(other.capacity_, capacity_);
    }

    SimpleVector& operator=(const SimpleVector& rhs) {
        if (this != &rhs) {
            SimpleVector<Type> rhs_copy(rhs);
            swap(rhs_copy);
        }
        return *this;
    }

    SimpleVector& operator=(SimpleVector&& rhs) {
        vector_.swap(rhs.vector_);
        std::swap(size_, rhs.size_);
        std::swap(capacity_, rhs.capacity_);
        return *this;
    }

    SimpleVector(ReserveProxyObj obj) : SimpleVector() {
        Reserve(obj.capacity_to_reserve_);
    }

    void Reserve(size_t new_capacity){
        if (new_capacity > capacity_){
            SimpleVector<Type> temp(new_capacity);
            std::copy(std::make_move_iterator(begin()),
                      std::make_move_iterator(end()),
                      temp.begin());
            temp.size_ = size_;
            swap(temp);
        }
    }

    size_t GetSize() const noexcept {
        return size_;
    }

    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    Type& operator[](size_t index) noexcept {
        assert(index < size_);
        return vector_[index];
    }

    const Type& operator[](size_t index) const noexcept {
        assert(index < size_);
        return vector_[index];
    }

    Type& At(size_t index) {
        if (index >= size_){
            throw std::out_of_range("out_of_range");
        }
        return vector_[index];
    }

    const Type& At(size_t index) const {
        if (index >= size_){
            throw std::out_of_range("out_of_range");
        }
        return vector_[index];
    }

    void Clear() noexcept {
        size_ = 0;
    }

    // Изменяет размер массива.
    void Resize(const size_t new_size) {
        if (new_size <= capacity_) {
            if (new_size > size_) {
                for(size_t i{0}; i < (capacity_ - size_) ; ++i){
                    PushBack(std::move(Type{}));
                }
            }
            size_ = new_size;
        } else {
            SimpleVector<Type> temp(std::max(capacity_ * 2, new_size));
            std::copy(std::make_move_iterator(begin()),
                      std::make_move_iterator(end()),
                      temp.begin());
            swap(temp);
        }
    }

    void swap(SimpleVector<Type> &other) noexcept {
        vector_.swap(other.vector_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    void PushBack(const Type& item) {
        if (size_ < capacity_) {
            vector_[size_] = std::move(item);
            ++size_;
        } else {
            SimpleVector<Type> temp(std::max(size_ * 2, static_cast<size_t>(1)));
            temp.size_ = size_ + 1;
            std::copy(std::make_move_iterator(begin()),
                      std::make_move_iterator(end()),
                      temp.begin());
            temp[size_] = std::move(item);
            swap(temp);
        }
    }

    void PushBack(Type&& item) {
        if (size_ < capacity_) {
            vector_[size_] = std::move(item);
            ++size_;
        } else {
            SimpleVector<Type> temp(std::max(size_ * 2, static_cast<size_t>(1)));
            temp.size_ = size_ + 1;
            std::copy(std::make_move_iterator(begin()),
                      std::make_move_iterator(end()),
                      temp.begin());
            temp[size_] = std::move(item);
            swap(temp);
        }
    }

    Iterator Insert(ConstIterator pos, const Type& value) {
        if (pos < begin() || pos > end()) {
            throw std::out_of_range("out of range");
        }
        auto index = static_cast<size_t> (pos - begin());
        if (index < capacity_){
            std::copy_backward(std::make_move_iterator(begin() + index),
                               std::make_move_iterator(end()),
                               end() + 1);
            vector_[index] = std::move(value);
            ++size_;
        }else{
            SimpleVector<Type> temp(std::max(size_ * 2, static_cast<size_t>(1)));
            temp.size_ = size_ + 1;
            std::copy(std::make_move_iterator(begin()),
                      std::make_move_iterator(end()),
                      temp.begin());
            temp[size_] = std::move(value);
            std::copy(std::make_move_iterator(begin() + index),
                      std::make_move_iterator(end()),
                      temp.begin() + index + 1);
            swap(temp);
        }
        return begin() + index;
    }

    Iterator Insert(ConstIterator pos, Type&& value) {
        if (pos < begin() || pos > end()) {
            throw std::out_of_range("out of range");
        }
        auto index = static_cast<size_t> (pos - begin());
        if (index < capacity_){
            std::copy_backward(std::make_move_iterator(begin() + index),
                               std::make_move_iterator(end()),
                               end() + 1);
            vector_[index] = std::move(value);
            ++size_;
        }else{
            SimpleVector<Type> temp(std::max(size_ * 2, static_cast<size_t>(1)));
            temp.size_ = size_ + 1;
            std::copy(std::make_move_iterator(begin()),
                      std::make_move_iterator(end()),
                      temp.begin());
            temp[size_] = std::move(value);
            std::copy(std::make_move_iterator(begin() + index),
                      std::make_move_iterator(end()),
                      temp.begin() + index + 1);
            swap(temp);
        }
        return begin() + index;
    }

    void PopBack() noexcept {
        assert(!IsEmpty());
        --size_;
    }

    Iterator Erase(ConstIterator pos) {
        if (pos < begin() || pos >= end()) {
            throw std::out_of_range("out of range");
        }
        auto index = static_cast<size_t>(pos - begin());
        std::copy(std::make_move_iterator(begin() + index + 1),
                  std::make_move_iterator(end()),
                  begin() + index);
        --size_;
        return begin() + index;
    }

    Iterator begin() noexcept {
        return vector_.Get();
    }

    Iterator end() noexcept {
        return vector_.Get() + size_;
    }

    ConstIterator begin() const noexcept {
        return vector_.Get();
    }

    ConstIterator end() const noexcept {
        return vector_.Get() + size_;
    }

    ConstIterator cbegin() const noexcept {
        return vector_.Get();
    }

    ConstIterator cend() const noexcept {
        return vector_.Get() + size_;
    }

private:
    ArrayPtr<Type> vector_;
    size_t size_ = 0;
    size_t capacity_ = 0;

};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(),
                                        rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !std::lexicographical_compare(rhs.begin(), rhs.end(),
                                         lhs.begin(), lhs.end());
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(rhs.begin(), rhs.end(),
                                        lhs.begin(), lhs.end());
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !std::lexicographical_compare(lhs.begin(), lhs.end(),
                                         rhs.begin(), rhs.end());
}
