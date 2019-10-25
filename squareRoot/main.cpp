#include <stdio.h>

double squareRoot2(size_t deep);

int main() {
    FILE *input = fopen("input.txt", "r");
    FILE *output = fopen("output.txt", "w");

    size_t depth = 0;
    fscanf(input, "%zu", &depth);

    double root = squareRoot2(depth);
    fprintf(output, "%.5lf\n", root);

    fclose(input);
    fclose(output);
}

double squareRoot2(size_t depth) {
    double value = 1;
    for(int i = 0; i < depth; i++) {
        value = 1 / (2 + value);
    }
    return value + 1;
}