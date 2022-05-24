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
    Optional() = default;
    // {
  /*  is_initialized_ = true;
     this->~Optional();*/
     // data_;
     // this->~Optional();
    //  delete[] data_;
 // }

    Optional(const T& value) {
        _Has_value = true;
        _Value = new (&data_[0]) T(value);

    }

    Optional(T&& value) {
        _Has_value = true;
        _Value = new (&data_[0]) T(std::move(value));
    }

    Optional(const Optional& other) {

        _Value = new (&data_[0]) T(*other._Value);
    }

    Optional(Optional&& other) {
        _Value = new (&data_[0]) T(std::move(*other._Value));
    }

    Optional& operator=(const T& value) {
        *_Value = value;
        return *this;
    }
    Optional& operator=(T&& rhs) {
        *_Value = std::move(rhs);
        return *this;
    }
    Optional& operator=(const Optional& rhs) {
        *this = rhs;
        return *this;
    }
    Optional& operator=(Optional&& rhs) {
        *this = std::move(rhs);
        return *this;
    }

    ~Optional() {
        /*if (is_initialized_) {
            new (&data_[0]) T(_Value);
        }*/
        // delete[] data_;
        // T();
         //if (is_initialized_) {
         //   // T();
         //}
         //is_initialized_ = false;
        // operator delete (data_);
    }

    bool HasValue() const {
        return _Has_value;
    }
    // Операторы * и -> не должны делать никаких проверок на пустоту Optional.
    // Эти проверки остаются на совести программиста

    T& operator*() {
        return *_Value;
    }

    const T& operator*() const {
        return this->_Value;
    }

    T* operator->() {

        return _Value;
    }

    const T* operator->() const {
        T* t = new (&data_[0]) T(_Value);
        return t;
    }

    // Метод Value() генерирует исключение BadOptionalAccess, если Optional пуст
    T& Value() {
        if (!this->_Has_value) {
            BadOptionalAccess();
        }

        return *_Value;
    }

    const T& Value() const {
        if (!this->_Has_value) {
            BadOptionalAccess();
        }

        return this->_Value;
    }

    void Reset() {
        _Has_value = false;
    }

private:

    // alignas нужен для правильного выравнивания блока памяти
    alignas(T) char data_[sizeof(T)];
    //   T* t = new (&data_[0]) T(_Value);
    bool is_initialized_ = false;
    bool _Has_value = false;
    T* _Value{};
};