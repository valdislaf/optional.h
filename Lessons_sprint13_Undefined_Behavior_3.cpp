#include <cstdint>
#include <iostream>

using namespace std;

class number {
private:
    int64_t m_value;
public:
    number() :m_value(0) {}
    number(int64_t value):m_value(value) {}
    number operator+(const number& right);
   
    friend istream& operator>>(istream& input, number& value) {
    
        input >> value.m_value;
       
        return input;
    }

    friend std::ostream& operator<< (std::ostream& out, const number& num) {
        out << num.m_value;
        return out;
    }
};

int main() {
    int64_t a;
    int64_t b;
    cin >> a >> b;
    if (((b > 0) && (a > (INT64_MAX - b))) ||
        ((b < 0) && (a < (INT64_MIN - b)))) {
        cout << "Overflow!"s;
    }
    else {
        cout << a + b << endl;
    }

   
}

number number::operator+(const number& right){ 
   this->m_value = this->m_value + right.m_value;
    return *this;
}

