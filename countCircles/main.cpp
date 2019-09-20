#define _USE_MATH_DEFINES
#include <iostream>
#include <math.h>
int fastpow(int a, int n);
using namespace std;
unsigned long long dumbFact(unsigned long long a);
unsigned long long fastFact(unsigned long long a);
int main() {
    for(int i = 0; i <= 30; i++) {
        cout << "dumbFact("<<i<<"), fastFact("<<i<<"), error: " << dumbFact(i) << " " << fastFact(i) << " " << 100.0f * (dumbFact(i) - fastFact(i)) / fastFact(i) << endl;
    }
}

unsigned long long dumbFact(unsigned long long a) {
    if (a == 0) return 1;
    else return a * dumbFact(a-1);
}

unsigned long long fastFact(unsigned long long a) {
    return (unsigned long long)(sqrt(2 * M_PI * a) * pow((double)(a / M_E), (double)a));
}


int fastpow(int a, int n) {
    if (n == 0) return 1;
    if (n % 2 == 1) return fastpow(a, n - 1) * a;
    else {
        int b = fastpow(a, n/2);
        return b * b;
    }
}