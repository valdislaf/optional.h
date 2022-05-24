#pragma once

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <stdexcept>
#include <utility>
#include <optional>

template <typename Type>
class ArrayPtr {
public:
    ArrayPtr() = default;

    explicit ArrayPtr(size_t size)
        : raw_ptr_(size != 0 ? new Type[size] : nullptr) {
    }

    explicit ArrayPtr(Type* raw_ptr) noexcept
        : raw_ptr_(raw_ptr) {
    }

    ArrayPtr(const ArrayPtr&) = delete;

    ArrayPtr(ArrayPtr&& other) {
        raw_ptr_ = std::exchange(other.raw_ptr_, nullptr);
    }

    ~ArrayPtr() {
        delete[] raw_ptr_;
    }

    ArrayPtr& operator=(const ArrayPtr&) = delete;

    ArrayPtr& operator=(ArrayPtr&& other) {
        raw_ptr_ = std::exchange(other.raw_ptr_, nullptr);
        return *this;
    }

    Type* Release() noexcept {
        Type* p = raw_ptr_;
        raw_ptr_ = nullptr;
        return p;
    }

    Type& operator[](size_t index) noexcept {
        return raw_ptr_[index];
    }

    const Type& operator[](size_t index) const noexcept {
        return raw_ptr_[index];
    }

    explicit operator bool() const {
        return raw_ptr_ != nullptr;
    }

    Type* Get() const noexcept {
        return raw_ptr_;
    }

    void swap(ArrayPtr& other) noexcept {
        std::swap(raw_ptr_, other.raw_ptr_);
    }

private:
    Type* raw_ptr_ = nullptr;
};

struct ReserveProxyObj {
    explicit ReserveProxyObj(size_t capacity_to_reserve)
        : capacity(capacity_to_reserve) {
    }
    size_t capacity;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
class SimpleVector {
    using ItemsPtr = ArrayPtr<Type>;

public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    // ������ ������ �� size ���������, ������������������ ��������� �� ���������
    explicit SimpleVector(size_t size)
        : SimpleVector(size,
            Type{})  // ���������� ������������� ������������, ������������ size � value
    {
    }

    // ������ ������ �� size ���������, ������������������ ��������� value
    SimpleVector(size_t size, const Type& value)
        : items_(size)  // ����� ������� ����������
        , size_(size)
        , capacity_(size)  //
    {
        std::fill(items_.Get(), items_.Get() + size_, value);  // ����� ������� ����������
    }

    // ������ ������ �� initializer_list
    SimpleVector(std::initializer_list<Type> init)
        : items_(init.size())  // ����� ������� ����������
        , size_(init.size())
        , capacity_(init.size())  //
    {
        std::copy(init.begin(), init.end(), items_.Get());
    }

    SimpleVector(ReserveProxyObj reserved)
        : items_(reserved.capacity)
        , size_(0)
        , capacity_(reserved.capacity) {
    }

    SimpleVector(const SimpleVector& other)
        : items_(other.size_)  // ����� ������� ����������
        , size_(other.size_)
        , capacity_(other.size_)  //
    {
        std::copy(other.items_.Get(), other.items_.Get() + size_,
            items_.Get());  // ����� ������� ����������
    }

    SimpleVector& operator=(const SimpleVector& rhs) {
        if (&rhs != this) {  // ����������� ������������ ������� ������ ����
            if (rhs.IsEmpty()) {
                // ����������� ��� ������ ������������ ������� �������
                Clear();
            }
            else {
                // ��������� ������ Copy-and-swap
                SimpleVector rhs_copy(rhs);  // ����� ������� ����������
                swap(rhs_copy);
            }
        }
        return *this;
    }

    // ���������� ���������� ��������� � �������
    size_t GetSize() const noexcept {
        return size_;
    }

    // ���������� ����������� �������
    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    // ��������, ������ �� ������
    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    // ���������� ������ �� ������� � �������� index
    Type& operator[](size_t index) noexcept {
        assert(index < size_);
        return items_[index];
    }

    // ���������� ����������� ������ �� ������� � �������� index
    const Type& operator[](size_t index) const noexcept {
        assert(index < size_);
        return items_[index];
    }

    // ���������� ����������� ������ �� ������� � �������� index
    // ����������� ���������� std::out_of_range, ���� index >= size
    Type& At(size_t index) {
        if (index >= size_) {
            using namespace std;
            throw out_of_range("Item index is out of range"s);
        }
        return items_[index];
    }

    // ���������� ����������� ������ �� ������� � �������� index
    // ����������� ���������� std::out_of_range, ���� index >= size
    const Type& At(size_t index) const {
        if (index >= size_) {
            using namespace std;
            throw out_of_range("Item index is out of range"s);
        }
        return items_[index];
    }

    // �������� ������ �������, �� ������� ��� �����������
    void Clear() noexcept {
        size_ = 0;
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            auto new_items = ReallocateCopy(new_capacity);  // ����� ������� ����������

            items_.swap(new_items);
            capacity_ = new_capacity;
        }
    }

    // �������� ������ �������.
    // ��� ���������� ������� ����� �������� �������� �������� �� ��������� ��� ���� Type
    void Resize(size_t new_size) {
        if (new_size > capacity_) {
            // ��������� ����������� �������
            const size_t new_capacity = std::max(capacity_ * 2, new_size);

            // �������� ������������ �������� ������� �� ����� �����
            auto new_items = ReallocateCopy(new_capacity);  // ����� ������� ����������
            // ��������� ����������� �������� ��������� �� ���������
            std::fill(new_items.Get() + size_, new_items.Get() + new_size,
                Type{});  // ����� ������� ����������

            items_.swap(new_items);
            capacity_ = new_capacity;
        }
        else if (new_size > size_) {
            assert(new_size <= capacity_);
            std::fill(items_.Get() + size_, items_.Get() + new_size,
                Type{});  // ����� ������� ����������
        }
        size_ = new_size;
    }

    // ��������� ������� � ����� �������
    // ��� �������� ����� ����������� ����� ����������� �������
    void PushBack(const Type& item) {
        const size_t new_size = size_ + 1;
        if (new_size > capacity_) {
            const size_t new_capacity = std::max(capacity_ * 2, new_size);

            auto new_items = ReallocateCopy(new_capacity);  // ����� ������� ����������
            new_items.Get()[size_] = item;  // ����� ������� ����������

            capacity_ = new_capacity;
            items_.swap(new_items);
        }
        else {
            items_.Get()[size_] = item;
        }
        size_ = new_size;
    }

    // ��������� �������� value � ������� pos.
    // ���������� �������� �� ����������� ��������
    // ���� ����� �������� �������� ������ ��� �������� ���������,
    // ����������� ������� ������ ����������� �����, � ��� ������� ������������ 0 ����� ������ 1
    Iterator Insert(ConstIterator pos, const Type& value) {
        assert(begin() <= pos && pos <= end());
        size_t new_size = size_ + 1;
        size_t new_item_offset = pos - cbegin();
        if (new_size <= capacity_) {  // ����������� ������� ���������� ��� ������� ��������
            Iterator mutable_pos = begin() + new_item_offset;

            // �������� ��������, ������� � ����������, ����� ��������� ������ ������
            std::copy_backward(pos, cend(),
                end() + 1);  // ����� ��������� ����������
            *mutable_pos = value;           // ����� ��������� ����������

        }
        else {  // ��������� ������������ ������
            size_t new_capacity = std::max(capacity_ * 2, new_size);

            ItemsPtr new_items(new_capacity);  // ����� ��������� ����������
            Iterator new_items_pos = new_items.Get() + new_item_offset;

            // �������� ��������, �������������� ������������
            std::copy(cbegin(), pos, new_items.Get());  // ����� ��������� ����������
            // ��������� ������� � ������� �������
            *new_items_pos = value;  // ����� ��������� ����������
            // �������� ��������, ��������� �� �����������
            std::copy(pos, cend(), new_items_pos + 1);  // ����� ��������� ����������

            items_.swap(new_items);
            capacity_ = new_capacity;
        }
        size_ = new_size;
        return begin() + new_item_offset;
    }

    // �������� ������� � ����� �������, �� �������� ��� �����������
    void PopBack() noexcept {
        assert(!IsEmpty());
        --size_;
    }

    // ������� ������� ������� � ������� pos, ������� ��������� �������� �� ��� �����.
    // �������� pos ������ ���� ����������, ����������� �� ������������ ������� �������.
    // ���������� �������� �� �������, ������� ������� �� ��������� �������� ���������.
    // ���� ��� ����� ��������� �������, ������ ��������� �������� end().
    Iterator Erase(ConstIterator pos) {
        assert(begin() <= pos && pos < end());
        Iterator mutable_pos = begin() + (pos - cbegin());

        // ��������� ������ �����, �� ����� ���������� ��������
        std::copy(mutable_pos + 1, end(), mutable_pos);  // ����� ��������� ����������

        --size_;
        return mutable_pos;
    }

    // ���������� �������� � ������ ��������
    void swap(SimpleVector& other) noexcept {
        items_.swap(other.items_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    // ���������� �������� �� ������ �������
    // ��� ������� ������� ����� ���� ����� (��� �� �����) nullptr
    Iterator begin() noexcept {
        return items_.Get();
    }

    // ���������� �������� �� �������, ��������� �� ���������
    // ��� ������� ������� ����� ���� ����� (��� �� �����) nullptr
    Iterator end() noexcept {
        return items_.Get() + size_;
    }

    // ���������� ����������� �������� �� ������ �������
    // ��� ������� ������� ����� ���� ����� (��� �� �����) nullptr
    ConstIterator begin() const noexcept {
        return items_.Get();
    }

    // ���������� �������� �� �������, ��������� �� ���������
    // ��� ������� ������� ����� ���� ����� (��� �� �����) nullptr
    ConstIterator end() const noexcept {
        return items_.Get() + size_;
    }

    // ���������� ����������� �������� �� ������ �������
    // ��� ������� ������� ����� ���� ����� (��� �� �����) nullptr
    ConstIterator cbegin() const noexcept {
        return items_.Get();
    }

    // ���������� �������� �� �������, ��������� �� ���������
    // ��� ������� ������� ����� ���� ����� (��� �� �����) nullptr
    ConstIterator cend() const noexcept {
        return items_.Get() + size_;
    }

private:
    // �������� ����� �������� ������� � �������� ������������
    ItemsPtr ReallocateCopy(size_t new_capacity) const {
        ItemsPtr new_items(new_capacity);  // ����� ������� ����������
        size_t copy_size = std::min(new_capacity, size_);
        std::copy(items_.Get(), items_.Get() + copy_size,
            new_items.Get());  // ����� ������� ����������
        return ItemsPtr(new_items.Release());
    }

    ItemsPtr items_;
    size_t size_ = 0;
    size_t capacity_ = 0;
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (lhs.GetSize() == rhs.GetSize())
        && std::equal(lhs.begin(), lhs.end(), rhs.begin());  // ����� ������� ����������
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);  // ����� ������� ����������
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(),
        rhs.end());  // ����� ������� ����������
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs < lhs);  // ����� ������� ����������
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs < lhs;  // ����� ������� ����������
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs <= lhs;  // ����� ������� ����������
}
