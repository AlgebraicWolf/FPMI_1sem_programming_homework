#include <iostream>

bool Compare(double a, double b);

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}

//Sort 1

void Sort(double data[], int n, bool compare()) {
    for(int i = 0 ; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if(Compare(data[i], data[j]));
        }
    }

}