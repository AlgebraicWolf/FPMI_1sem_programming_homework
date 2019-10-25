#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

double dot(double *a, double *b, int n);

double angle(double *a, double *b, int n);

int main() {
    FILE *input = fopen("input.txt", "r");
    FILE *output = fopen("output.txt", "w");
    int n = 0;
    double *a = nullptr;
    double *b = nullptr;
    fscanf(input, "%d\n", &n);
    a = (double *) calloc(n, sizeof(double));
    b = (double *) calloc(n, sizeof(double));
    double comp = 0;
    for (int i = 0; i < n; i++) {
        fscanf(input, "%lf", &comp);
        a[i] = comp;
    }

    fscanf(input, "\n");

    for (int i = 0; i < n; i++) {
        fscanf(input, "%lf", &comp);
        b[i] = comp;
    }
    char command[6] = {};
    fscanf(input, "\n");
    fscanf(input, "%s", command);
    fclose(input);
    double result = -1;
    if (strcmp(command, "dot") == 0)
        result = dot(a, b, n);
    else if (strcmp(command, "angle") == 0)
        result = angle(a, b, n);

    fprintf(output, "%.2lf", result);
    fclose(output);
    free(a);
    free(b);
    return 0;
}

double dot(double *a, double *b, int n) {
    double dotProduct = 0;
    for (int i = 0; i < n; i++) {
        dotProduct += a[i] * b[i];
    }
    return dotProduct;
}

double angle(double *a, double *b, int n) {
    return acos(dot(a, b, n) / (sqrt(dot(a, a, n) * dot(b, b, n))));
}