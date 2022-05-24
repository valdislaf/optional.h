#include <climits>
#include <iomanip>
#include <iostream>
#include <optional>

bool TestSigned(int n) {
    return n < n + 1;
}

bool TestUnsigned(unsigned int n) {
    return n < n + 1;
}

using namespace std;
int main() {
    cout << boolalpha;
    cout << TestSigned(INT_MAX) << endl;
    cout << TestUnsigned(UINT_MAX) << endl;
}