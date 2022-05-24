#include <stdexcept>
#include <utility>

using namespace std::literals;
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
    Optional(const T& value);
    Optional(T&& value);
    Optional(const Optional& other);
    Optional(Optional&& other);

    Optional& operator=(const T& value) {
        this->Value() = value;
    return  *this;
    }

    Optional& operator=(T&& rhs) {
       
        this->Value() = std::forward<T>(rhs);
        return  *this;
    }

    Optional& operator=(const Optional& rhs) {
        *this = rhs;
        return  *this;
    }

    Optional& operator=(Optional&& rhs) {
        *this = std::move(rhs);
        return  *this;
    }

    ~Optional();

    bool HasValue() const;

    // Операторы * и -> не должны делать никаких проверок на пустоту Optional.
    // Эти проверки остаются на совести программиста
    T& operator*();
    const T& operator*() const;
    T* operator->();
    const T* operator->() const;

    // Метод Value() генерирует исключение BadOptionalAccess, если Optional пуст
    T& Value() {
        if (!this->HasValue()) {
            throw std::invalid_argument("Bad optional access"s);
        }

        return this->Value();
    }
    const T& Value() const {
        if (!this->HasValue()) {
            throw std::invalid_argument("Bad optional access"s);
        }

        return this->Value();
    }

    void Reset();

private:
    bool _Has_value = false;
    // alignas нужен для правильного выравнивания блока памяти
    alignas(T) char data_[sizeof(T)];
    bool is_initialized_ = false;
};


template<typename T>
inline Optional<T>::Optional(const T& value)
{
    this->_Has_value = true;
   Optional<T> opt{};
   opt._Has_value = true;
   *this = std::move(opt);
  //opt.Value() = value;
    //if (!this->_Has_value) {
    //  //  this->Value() = nullptr;
    //}else{ 
    //    this = std::move(opt);
    //}
   
  
   
    
    /*std::swap(value, this->Value());*/
    
}

template<typename T>
inline Optional<T>::Optional(T&& value)
{
    this->_Has_value = true;
    this->Value() = value;
    
}

template<typename T>
inline Optional<T>::Optional(const Optional& other)
{
    this->_Has_value = true;
    *this =other;
    

}

template<typename T>
inline Optional<T>::Optional(Optional&& other)
{
    this->_Has_value = true;
    *this = std::move(other);
   
}

//template<typename T>
//inline Optional& Optional<T>::operator=(const T& value)
//{   
//    Optional ot(value);
//    return *this;
//}
//
//template<typename T>
//inline Optional& Optional<T>::operator=(T&& rhs)
//{
//    return *this;
//}
//
//template<typename T>
//inline Optional& Optional<T>::operator=(const Optional& rhs)
//{
//    return *this;
//}
//
//template<typename T>
//inline Optional& Optional<T>::operator=(Optional&& rhs)
//{
//    return *this;
//}

template<typename T>
inline Optional<T>::~Optional()
{
}

 template<typename T>
inline bool Optional<T>::HasValue() const
{
    return this->_Has_value;
}

template<typename T>
inline T& Optional<T>::operator*()
{
    return this->Value();
}

template<typename T>
inline const T& Optional<T>::operator*() const
{
    return this->Value();
}

template<typename T>
inline T* Optional<T>::operator->()
{
    return nullptr;
}

template<typename T>
inline const T* Optional<T>::operator->() const
{
    return nullptr;
}


template<typename T>
inline void Optional<T>::Reset()
{
    this->_Has_value = false;
}
