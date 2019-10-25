#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

struct matrix {
    int n;
    int m;
    double *data;
};

matrix makeMatrix(int n, int m);

matrix mulMatrix(matrix mat1, matrix mat2);

matrix makeRotMatrix(char axis, double angle);

void resizeVectorByValue(matrix V, double value);

void printMatrix(FILE *f, matrix M);

void setMatrix(matrix M, int i, int j, double val);

matrix transposeMatrix(matrix M);

int main() {
    FILE *input = fopen("input.txt", "r");
    FILE *output = fopen("output.txt", "w");

    matrix v = makeMatrix(3, 1);
    setMatrix(v, 1, 0, 1);

    char cmd[64] = {};
    char axis = 0;
    double arg = 0;
    matrix M = {};
    while(fscanf(input, "%s ", cmd) != EOF) {
        if(strcmp(cmd, "rot") == 0) {
            fscanf(input, "%c %lf", &axis, &arg);
            M = makeRotMatrix(axis, arg);
            v = mulMatrix(M, v);
        }
        else if(strcmp(cmd, "len") == 0) {
            fscanf(input, "%lf", &arg);
            resizeVectorByValue(v, arg);
        }
        fscanf(input, "\n");
    }
    v = transposeMatrix(v);
    printMatrix(output, v);
    fclose(input);
    fclose(output);
}

matrix makeMatrix(int n, int m) {
    matrix M = {};
    M.n = n;
    M.m = m;
    M.data = (double *) calloc(n * m, sizeof(double));
    return M;
}

matrix mulMatrix(matrix mat1, matrix mat2) {
     assert(mat1.m == mat2.n);
     assert(mat1.data);
     assert(mat2.data);
     double res;
     auto resMatrix = makeMatrix(mat1.n, mat2.m);
     for(int ires = 0; ires < mat1.n; ires++) {
         for(int jres = 0; jres < mat2.m; jres++) {
             res = 0;
             for (int k = 0; k < mat1.m; k++)
                 res += mat1.data[ires * mat1.m + k] * mat2.data[k * mat2.m + jres];
             resMatrix.data[ires * resMatrix.m + jres] = res;
         }
     }
     return resMatrix;
}

void printMatrix(FILE *f, matrix M) {
    assert(M.data);
    for (int i = 0; i < M.n; i++) {
        for (int j = 0; j < M.m; j++) {
            if (fabs(*(M.data+i*M.m+j)) < 0.1)
                fprintf(f, "%.1lf ", (double)0);
            else
                fprintf(f, "%.1lf ", *(M.data+i*M.m+j));
        }
        fprintf(f, "\n");
    }
}

void setMatrix(matrix M, int i, int j, double val) {
    assert(i < M.n);
    assert(j < M.m);
    assert(M.data);
    *(M.data + i * M.m + j) = val;
}

matrix makeRotMatrix(char axis, double angle) {
    matrix M = makeMatrix(3, 3);
    switch (toupper(axis)) {
        case 'X':
            setMatrix(M, 0, 0, 1);
            setMatrix(M, 1, 1, cos(angle));
            setMatrix(M, 1, 2, -sin(angle));
            setMatrix(M, 2, 1, sin(angle));
            setMatrix(M, 2, 2, cos(angle));
            break;
        case 'Y':
            setMatrix(M, 1, 1, 1);
            setMatrix(M, 0, 0, cos(angle));
            setMatrix(M, 0, 2, sin(angle));
            setMatrix(M, 2, 0, -sin(angle));
            setMatrix(M, 2, 2, cos(angle));
            break;
        case 'Z':
            setMatrix(M, 2, 2, 1);
            setMatrix(M, 0, 0, cos(angle));
            setMatrix(M, 0, 1, -sin(angle));
            setMatrix(M, 1, 0, sin(angle));
            setMatrix(M, 1, 1, cos(angle));
            break;
        default:
            abort();
    }
    return M;
}

void resizeVectorByValue(matrix V, double value) {
    assert(V.n == 3);
    assert(V.m == 1);
    assert(V.data);
    double l = sqrt(*V.data * *V.data  + *(V.data + 1) * *(V.data + 1) + *(V.data + 2) * *(V.data + 2));
    double factor = (l + value) / l;
    *V.data *= factor;
    *(V.data + 1) *= factor;
    *(V.data + 2) *= factor;
}

matrix transposeMatrix(matrix M) {
    matrix MT = makeMatrix(M.m, M.n);
        for (int i = 0; i < M.n; i++)
            for (int j = 0; j < M.m; j++)
                *(MT.data + j * MT.m + i) = *(M.data + i * M.m + j);
    return MT;
}