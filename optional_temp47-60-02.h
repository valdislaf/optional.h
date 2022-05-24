#pragma once
#include <stdexcept>
#include <utility>

// Исключение этого типа должно генерироватся при обращении к пустому optional
class BadOptionalAccess : public std::exception {
public:
    using exception::exception;

    virtual const char* what() const noexcept override {
        return "Bad optional access";
    }
};

template <typename T>
class Optional {
public:
    Optional() {
        is_initialized_ = false;
    }

    Optional(const T& value) {
        if (value_ != nullptr) {
            *value_ = value;
        }
        else { value_ = new (data_) T(value); }
        is_initialized_ = true;

    }

    Optional(T&& value) {
        if (value_ != nullptr) {
            *value_ = std::move(value);
        }
        else { value_ = new (data_) T(std::move(value)); }
        is_initialized_ = true;

    }

    Optional(const Optional& other) {
        if (value_ != nullptr) {
            *value_ = *other.value_;
        }
        else { value_ = new (data_) T(*other.value_); }
        is_initialized_ = true;
    }

    Optional(Optional&& other) {
        if (value_ != nullptr) {
            *value_ = std::move(*other.value_);
        }
        else { value_ = new (data_) T(std::move(*other.value_)); }
        is_initialized_ = true;
    }

    Optional& operator=(const T& value) {
        if (value_ != nullptr) { *value_ = value; }
        else {
            value_ = new (data_) T(value);
        }
        is_initialized_ = true;
        return *this;
    }

    Optional& operator=(T&& rhs) {
        if (value_ != nullptr) { *value_ = std::move(rhs); }
        else {
            value_ = new (data_) T(std::move(rhs));
        }
        is_initialized_ = true;
        return *this;
    }

    Optional& operator=(const Optional& rhs) {

        if (!is_initialized_ && !rhs.is_initialized_) {
            value_->~T();
            is_initialized_ = false;

        }

        else  if (!is_initialized_ && rhs.is_initialized_) {
            value_ = new (data_) T(*rhs.value_);
            is_initialized_ = true;

        }

        else if (is_initialized_ && rhs.is_initialized_) {
            *value_ = *rhs.value_;
            is_initialized_ = true;
        }

        else  if (is_initialized_ && !rhs.is_initialized_) {
            if (rhs.value_ != nullptr) {
                std::swap(*value_, *rhs.value_);
            }
            else {
                value_->~T();
            }
            is_initialized_ = false;
        }

        return *this;
    }

    Optional& operator=(Optional&& rhs) {
        if (!is_initialized_ && !rhs.is_initialized_) {
            value_->~T();
            is_initialized_ = false;
        }
        else if (!is_initialized_ && rhs.is_initialized_) {
            value_ = new (data_) T(std::move(*rhs.value_));
            is_initialized_ = true;

        }
        else if (is_initialized_ && rhs.is_initialized_) {// Assign non empty to non-empty
            *value_ = std::move(*rhs.value_);
        }
        else  if (is_initialized_ && !rhs.is_initialized_) {
            value_->~T();

            is_initialized_ = false;

        }
        return *this;
    }

    ~Optional() {
        if (value_ != nullptr) {
            if (is_initialized_) {
                value_->~T();
            }
        }
        is_initialized_ = false;
    }

    bool HasValue() const {
        return is_initialized_;
    }
    // Операторы * и -> не должны делать никаких проверок на пустоту Optional.
    // Эти проверки остаются на совести программиста

    T& operator*() {
        return *value_;
    }

    const T& operator*() const {
        return *this->value_;
    }

    T* operator->() {
        return reinterpret_cast<T*>(value_);
    }

    const T* operator->() const {
        return reinterpret_cast<const T*>(value_);
    }

    // Метод Value() генерирует исключение BadOptionalAccess, если Optional пуст
    T& Value() {
        if (!this->is_initialized_) {
            throw BadOptionalAccess();
        }
        return *value_;
    }

    const T& Value() const {
        if (!this->is_initialized_) {
            throw BadOptionalAccess();
        }
        return  *this->value_;
    }

    void Reset() {
        is_initialized_ = false;
    }

private:

    // alignas нужен для правильного выравнивания блока памяти
    alignas(T) char data_[sizeof(T)];

    bool is_initialized_ = false;

    T* value_{};

    // reinterpret_cast<T&>(data_) = value;
};