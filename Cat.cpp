#include <iostream>
#include <memory>
#include <new>
#include <string>

using namespace std;

class Cat {
public:
    Cat(string name, int age)
        : name_(move(name))
        , age_(age)  //
    {
        cout << "Hello from " << name_ << endl;
    }

    ~Cat() {
        cout << "Goodbye from "sv << name_ << endl;
    }

    void SayHello() const {
        cout << "Meow, my name is "sv << name_ << ". I'm "sv << age_ << " year old."sv << endl;
    }

private:
    string name_;
    int age_;
};

int main() {
    {
        Cat cat("Tom"s, 2);
        cat.SayHello();
    }
    {
        Cat* cat = new Cat("Leo"s, 3);
        cat->SayHello();
        delete cat;
    }
    {
        auto cat = make_unique<Cat>("Felix"s, 4);
        cat->SayHello();
    }
    {
        alignas(Cat) char buf[sizeof(Cat)]{};
        Cat* cat = new (&buf[0]) Cat("Luna"s, 1);
       // unique_ptr<Cat> cat(new (&buf[0]) Cat("Luna"s, 1)); //Объект типа Cat создан размещающим оператором new. Чтобы удалить Cat, нужно использовать явный вызов деструктора.
        cat->SayHello();
        cat->~Cat();
    }
    {
        void* buf = operator new (sizeof(Cat));
        size_t s = sizeof(Cat);//48
        Cat* cat = new (buf) Cat("Murka"s, 4);
        cat->SayHello();
        cat->~Cat();
        operator delete (buf);
    }
}