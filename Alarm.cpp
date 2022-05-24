#include <climits>
#include <iomanip>
#include <iostream>
#include <functional>

using namespace std;

// Указатель на функцию без параметров, которая возвращает void
//using Alarm = void (*)();


using Alarm = function<void()>;
namespace {
    // Поведение сигнализации определяется значением указателя alarm
    Alarm alarm = nullptr;
}  // namespace

// Выпустить велоцираптора
void HostileAlarm() {
    cout << "Attack with velociraptor"s << endl;
}

// Настраиваем сигнализацию на выпуск велоцираптора
void SetHostileAlarm() {
    cout << "Hostile alarm set"s << endl;
    alarm = HostileAlarm;
}

void Test() {
    if (alarm != nullptr) {
        alarm();
    }
    cout << "Test succeeded"sv << endl;
}

bool TestSigned(int n) {
    return n < n + 1;
}

bool TestUnsigned(unsigned int n) {
    return n < n + 1;
}

using namespace std;
int main() {

    Test();

    /*cout << boolalpha;
    cout << TestSigned(INT_MAX) << endl;
    cout << TestUnsigned(UINT_MAX) << endl;*/
}