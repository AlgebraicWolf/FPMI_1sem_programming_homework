#include <stdio.h>
long f(size_t n);

int main() {
    FILE *input = fopen("input.txt", "r");
    FILE *output = fopen("output.txt", "w");

    size_t n = 0;
    fscanf(input, "%zu", &n);
    fprintf(output, "%ld", f(n));

    fclose(input);
    fclose(output);
    return 0;
}

long f(size_t n) {
    if (n == 0) return 3;
    else if (n == 1) return 14;
    else if (n == 2) return 94;
    else if (n == 3) return 764;
    else return 14 * f(n-1) - 51 * f(n-2) + 54 * f(n-3);
}