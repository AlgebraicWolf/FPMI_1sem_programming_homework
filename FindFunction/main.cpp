#include <stdio.h>
#include <math.h>
#include <stdlib.h>

double *F(double x, double y, double a, double b, double c);
double MSE(double Fx, double Fy, double _Fx, double _Fy);

int main() {
    FILE *input = fopen("input.txt", "r");
    FILE *output = fopen("output.txt", "w");

    int n_points = 9;
    auto xs = (double *) calloc(n_points, sizeof(double));
    auto ys = (double *) calloc(n_points, sizeof(double));
    auto Fxs = (double *) calloc(n_points, sizeof(double));
    auto Fys = (double *) calloc(n_points, sizeof(double));

    for (int i = 0; i < n_points; i++) {
        fscanf(input, "%lf\t%lf\t%lf\t%lf\n", xs + i, ys + i, Fxs + i, Fys + i);
    }
    fclose(input);

    double minMSE = 100000000000;
    double mina = 0;
    double minb = 0;
    double minc = 0;

    double *F_val = nullptr;
    double totalMSE = 0;

    for (double a = 8; a <= 12; a += 0.1)
        for (double b = 8; b <= 12; b += 0.1)
            for (double c = 0.8; c <= 1.2; c += 0.1) {
                totalMSE = 0;
                for (int i = 0; i < n_points; i++) {
                    F_val = F(xs[i], ys[i], a, b, c);
                    totalMSE += MSE(F_val[0], F_val[1], Fxs[i], Fys[i]);
                }
                totalMSE /= 9;
                totalMSE = sqrt(totalMSE);
                printf("a=%.1lf b=%.1lf c=%.1lf MSE=%lf\n", a, b, c, totalMSE);
                if (totalMSE < minMSE) {

                    mina = a;
                    minb = b;
                    minc = c;
                    minMSE = totalMSE;
                }
            }

    fprintf(output, "%.1lf %.1lf %.1lf", mina, minb, minc);
    fclose(output);
    free(xs);
    free(ys);
    free(Fxs);
    free(Fys);
    return 0;
}

double *F(double x, double y, double a, double b, double c) {
    double *result = (double *) malloc(2 * sizeof(double));
    result[0] = 1 - pow(x / a, 2) - pow((y / b), 2);
    result[1] = c * x / y;
    return result;
}

double MSE(double Fx, double Fy, double _Fx, double _Fy) {
    return (Fx - _Fx) * (Fx - _Fx) + (Fy - _Fy) * (Fy - _Fy);
}