#include <stdio.h>

double sec(int n);

int main() {
    FILE *input = fopen("input.txt", "r");
    FILE *output = fopen("output.txt", "w");

    int n = 0;
    fscanf(input, "%d", &n);
    fprintf(output, "%.3lf\n", sec(n));

    fclose(input);
    fclose(output);
    return 0;
}

double sec(int n) {
    if (n == 0)
        return 1;
    else return 1 + 1 / sec(n-1);
}