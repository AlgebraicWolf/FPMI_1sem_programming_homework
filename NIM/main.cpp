#include <stdio.h>

int main() {
    FILE *input = fopen("input.txt", "r");
    FILE *output = fopen("output.txt", "w");

    unsigned long long N = 0;
    fscanf(input, "%llu", &N);
    int extra = (N - 1) % 5;
    if (extra == 0) {
        fprintf(output, "100"); // PANIC MODE, this should never ever happen, added for logical integrity
    }
    else {
        fprintf(output, "%d", extra);
    }

    fclose(input);
    fclose(output);
    return 0;
}