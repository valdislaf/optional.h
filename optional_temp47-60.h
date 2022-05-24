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

    Optional(const T& value) {
        is_initialized_ = true;
        _Has_value = true;
        _Value = new (&data_[0]) T(value);

    }

    Optional(T&& value) {
        is_initialized_ = true;
        _Has_value = true;
        _Value = new (&data_[0]) T(std::move(value));
    }

    Optional(const Optional& other) {
        is_initialized_ = true;
        _Has_value = true;
        _Value = new (&data_[0]) T(*other._Value);
    }

    Optional(Optional&& other) {
        is_initialized_ = true;
        _Has_value = true;
        _Value = new (&data_[0]) T(std::move(*other._Value));
    }

    Optional& operator=(const T& value) {
        if (_Value != nullptr) { *_Value = value; }
        else {

            _Value = new (&data_[0]) T(value);
        }
        is_initialized_ = true;
        _Has_value = true;
        return *this;
    }

    Optional& operator=(T&& rhs) {
        if (_Value != nullptr) { *_Value = std::move(rhs); }
        else {
            _Value = new (&data_[0]) T(std::move(rhs));
        }

        is_initialized_ = true;

        _Has_value = true;
        return *this;
    }
    Optional& operator=(const Optional& rhs) {

        if (!_Has_value && !rhs._Has_value) {
            _Value->~T();
            // _Value = nullptr;
            is_initialized_ = false;
            _Has_value = false;

        }
        else  if (!_Has_value && rhs._Has_value) {
            _Value = new (&data_[0]) T(*rhs._Value);
            is_initialized_ = true;
            _Has_value = true;
        }

        else if (_Has_value && rhs._Has_value) {

            *_Value = *rhs._Value;
            is_initialized_ = true;
            _Has_value = true;
        }
        else  if (_Has_value && !rhs._Has_value) {
            if (rhs._Value != nullptr) {
                std::swap(*_Value, *rhs._Value);
            }
            else {
                _Value->~T();
                // _Value = nullptr; 
            }

            is_initialized_ = false;
            _Has_value = false;
        }

        return *this;
    }

    Optional& operator=(Optional&& rhs) {

        if (!_Has_value && !rhs._Has_value) {
            _Value->~T();
            //  _Value = nullptr;
            is_initialized_ = false;
            _Has_value = false;

        }
        else if (!_Has_value && rhs._Has_value) {
            _Value = new (&data_[0]) T(std::move(*rhs._Value));
            _Has_value = std::move(rhs._Has_value);
            is_initialized_ = std::move(rhs.is_initialized_);
        }
        else if (_Has_value && rhs._Has_value) {// Assign non empty to non-empty


            *_Value = std::move(*rhs._Value);



        }
        else  if (_Has_value && !rhs._Has_value) {
            _Value->~T();
            _Value = nullptr;
            /*this->~Optional();
            std::swap(_Value, rhs._Value);

            rhs.~Optional();*/

            is_initialized_ = false;
            _Has_value = false;
        }
        return *this;
    }

    ~Optional() {
        if (_Value != nullptr) {
            if (is_initialized_) _Value->~T();
        }
        is_initialized_ = false;

        _Has_value = false;
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
        return *this->_Value;
    }

    T* operator->() {

        return _Value;
    }

    const T* operator->() const {

        return reinterpret_cast<const T*>(data_);

    }

    // Метод Value() генерирует исключение BadOptionalAccess, если Optional пуст
    T& Value() {
        if (!this->_Has_value) {
            throw    BadOptionalAccess();
        }

        return *_Value;
    }

    const T& Value() const {
        if (!this->_Has_value) {
            throw     BadOptionalAccess();
        }

        return  *this->_Value;
    }

    void Reset() {
        _Has_value = false;
    }

private:

    // alignas нужен для правильного выравнивания блока памяти
    alignas(T) char data_[sizeof(T)];

    bool is_initialized_ = false;
    bool _Has_value = false;
    T* _Value{};
};