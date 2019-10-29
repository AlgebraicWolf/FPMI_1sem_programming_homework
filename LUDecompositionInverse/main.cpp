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

int LUDecomposition(matrix m, matrix* L, matrix* U);

matrix LUInversion(matrix l, matrix u);

matrix mergeMatrices(matrix m11, matrix m12, matrix m21, matrix m22);

matrix addMatrix(matrix mat1, matrix mat2);

matrix subMatrix(matrix mat1, matrix mat2);

double getMatrix(matrix M, int i, int j);

void splitMatrix(matrix mat, matrix *m11, matrix *m12, matrix *m21, matrix *m22);

void resizeVectorByValue(matrix V, double value);

void printMatrix(FILE *f, matrix M);

void setMatrix(matrix M, int i, int j, double val);

matrix transposeMatrix(matrix M);

int main() {
    FILE *input = fopen("input.txt", "r");
    FILE *output = fopen("output.txt", "w");
    int n = 0;
    double value = 0;
    fscanf(input, "%d", &n);
    matrix M = makeMatrix(n, n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            fscanf(input, "%lf", &value);
            setMatrix(M, i, j, value);
        }
        fscanf(input, "\n");
    }
    matrix l = {};
    matrix u = {};

    LUDecomposition(M, &l, &u);
    matrix solution = LUInversion(l, u);

    printMatrix(output, solution);

    fclose(input);
    fclose(output);
}

matrix solveTriangleSystem(matrix A, matrix b, int start, int propagation) {
    assert(A.data);
    assert(b.data);
    assert(A.m == b.n);
    assert(b.m == 1);
    assert(A.m == A.n);

    FILE *dumps = fopen("dumps.txt", "w");
    printMatrix(dumps, A);
    fprintf(dumps, "\n");
    printMatrix(dumps, b);
    fprintf(dumps, "\n");
    double sum = 0;
    matrix solution = makeMatrix(A.m, 1);
    for(int i = start; (i >= 0) && (i < A.m); i+= propagation) {
        sum = 0;
        for(int k = 0; k < A.n; k++) {
            if(k != i)
                sum += getMatrix(A, i, k) * getMatrix(solution, k, 0);
        }
        setMatrix(solution, i, 0, (getMatrix(b, i, 0) - sum) / getMatrix(A, i, i));
    }
    printMatrix(dumps, solution);
    fclose(dumps);
    return solution;
}

matrix LUInversion(matrix l, matrix u) {
    assert(l.data);
    assert(u.data);
    matrix m = makeMatrix(l.n, l.m);
    matrix target = {};
    matrix subsolution = {};
    for(int i = 0; i < l.n; i++) {
        target = makeMatrix(l.n, 1);
        setMatrix(target, i, 0, 1);
        subsolution = solveTriangleSystem(l, target, 0, 1);
        subsolution = solveTriangleSystem(u, subsolution, u.n - 1, -1);
        for(int j = 0; j < l.n; j++)
            setMatrix(m, j, i, getMatrix(subsolution, j, 0));
        free(target.data);
    }
    return m;
}

int LUDecomposition(matrix m, matrix* L, matrix* U) {
    assert(m.data);
    assert(m.n == m.m);
    matrix l = makeMatrix(m.n, m.m);
    matrix u = makeMatrix(m.n, m.m);
    for(int j = 0; j < m.n; j++)
        setMatrix(u, 0, j, getMatrix(m, 0, j));

    for(int i = 0; i < m.n; i++)
        setMatrix(l, i, 0, getMatrix(m, i, 0) / getMatrix(u, 0, 0));

    for(int i = 1; i < m.n; i++) {
        for(int j = i; j < m.n; j++) {
            setMatrix(u, i, j, getMatrix(m, i, j));
            for(int k = 0; k <= i - 1; k++)
                setMatrix(u, i, j, getMatrix(u, i, j) - getMatrix(l, i, k) * getMatrix(u, k, j));
        }
        for(int j = i; j < m.n; j++) {
            setMatrix(l, j, i, getMatrix(m, j, i));
            for(int k = 0; k <= i - 1; k++)
                setMatrix(l, j, i, getMatrix(l, j, i) - getMatrix(l, j, k) * getMatrix(u, k, i));
            setMatrix(l, j, i, getMatrix(l, j, i) / getMatrix(u, i, i));
        }
    }

    L->m = l.m;
    L->n = l.n;
    L->data = l.data;
    U->m = u.m;
    U->n = u.n;
    U->data = u.data;
    return 1;
}


matrix makeMatrix(int n, int m) {
    matrix M = {};
    M.n = n;
    M.m = m;
    M.data = (double *) calloc(n * m, sizeof(double));
    return M;
}


void splitMatrix(matrix mat, matrix *m11, matrix *m12, matrix *m21, matrix *m22) {
    assert(mat.m == mat.n);
    assert(mat.data);
    int splitSize = mat.n / 2;
    *m11 = makeMatrix(splitSize, splitSize);
    *m12 = makeMatrix(splitSize, splitSize);
    *m21 = makeMatrix(splitSize, splitSize);
    *m22 = makeMatrix(splitSize, splitSize);
    for (int i = 0; i < splitSize; i++) {
        for (int j = 0; j < splitSize; j++) {
            setMatrix(*m11, i, j, getMatrix(mat, i, j));
        }
    }

    for (int i = splitSize; i < mat.n; i++) {
        for (int j = 0; j < splitSize; j++) {
            setMatrix(*m21, i - splitSize, j, getMatrix(mat, i, j));
        }
    }

    for (int i = 0; i < splitSize; i++) {
        for (int j = splitSize; j < mat.n; j++) {
            setMatrix(*m12, i, j - splitSize, getMatrix(mat, i, j));
        }
    }

    for (int i = splitSize; i < mat.n; i++) {
        for (int j = splitSize; j < mat.n; j++) {
            setMatrix(*m22, i - splitSize, j - splitSize, getMatrix(mat, i, j));
        }
    }
}

matrix mergeMatrices(matrix m11, matrix m12, matrix m21, matrix m22) {
    assert(m11.n == m12.n);
    assert(m12.n == m21.n);
    assert(m12.n == m22.n);
    assert(m11.data);
    assert(m12.data);
    assert(m21.data);
    assert(m22.data);
    int mergedSize = 2 * m11.n;
    matrix mat = makeMatrix(mergedSize, mergedSize);

    for (int i = 0; i < m11.n; i++) {
        for (int j = 0; j < m11.n; j++) {
            setMatrix(mat, i, j, getMatrix(m11, i, j));
        }
    }

    for (int i = m11.n; i < mat.n; i++) {
        for (int j = 0; j < m11.n; j++) {
            setMatrix(mat, i, j, getMatrix(m21, i - m11.n, j));
        }
    }

    for (int i = 0; i < m11.n; i++) {
        for (int j = m11.n; j < mat.n; j++) {
            setMatrix(mat, i, j, getMatrix(m12, i, j - m11.n));
        }
    }

    for (int i = m11.n; i < mat.n; i++) {
        for (int j = m11.n; j < mat.n; j++) {
            setMatrix(mat, i, j, getMatrix(m22, i - m11.n, j - m11.n));
        }
    }

    return mat;
}

matrix mulMatrix(matrix mat1, matrix mat2) {
    assert(mat1.m == mat2.n);
    assert(mat1.data);
    assert(mat2.data);
    double res = 0;
    auto resMatrix = makeMatrix(mat1.n, mat2.m);
    for (int ires = 0; ires < mat1.n; ires++) {
        for (int jres = 0; jres < mat2.m; jres++) {
            res = 0;
            for (int k = 0; k < mat1.m; k++)
                res += mat1.data[ires * mat1.m + k] * mat2.data[k * mat2.m + jres];
            resMatrix.data[ires * resMatrix.m + jres] = res;
        }
    }
    return resMatrix;
}

matrix addMatrix(matrix mat1, matrix mat2) {
    assert(mat1.n == mat2.n);
    assert(mat1.m == mat2.m);
    assert(mat1.data);
    assert(mat2.data);
    matrix mat = makeMatrix(mat1.n, mat1.m);
    for (int i = 0; i < mat1.n; i++) {
        for(int j = 0; j < mat1.m; j++) {
            setMatrix(mat, i, j, getMatrix(mat1, i, j) + getMatrix(mat2, i, j));
        }
    }
    return mat;
}

matrix subMatrix(matrix mat1, matrix mat2) {
    assert(mat1.n == mat2.n);
    assert(mat1.m == mat2.m);
    assert(mat1.data);
    assert(mat2.data);
    matrix mat = makeMatrix(mat1.n, mat1.m);
    for (int i = 0; i < mat1.n; i++) {
        for(int j = 0; j < mat1.m; j++) {
            setMatrix(mat, i, j, getMatrix(mat1, i, j) - getMatrix(mat2, i, j));
        }
    }
    return mat;
}

void printMatrix(FILE *f, matrix M) {
    assert(M.data);
    for (int i = 0; i < M.n; i++) {
        for (int j = 0; j < M.m; j++) {
            if(fabs(getMatrix(M, i, j)) < 0.05)
                fprintf(f, "%.1lf ", (double) (0) );
            else
                fprintf(f, "%.1lf ", getMatrix(M, i, j));
        }
        fprintf(f, "\n");
    }
}

void setMatrix(matrix M, int i, int j, double val) {
    assert(i < M.n);
    assert(j < M.m);
    assert(M.data);
    M.data[i * M.m + j] = val;
}

double getMatrix(matrix M, int i, int j) {
    assert(i < M.n);
    assert(j < M.m);
    assert(M.data);
    return M.data[i * M.m + j];
}

matrix makeRotMatrix(char axis, int angle) {
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
    double l = sqrt(V.data[0] * V.data[0] + V.data[1] * V.data[1] + V.data[2] * V.data[2]);
    double factor = (l + value) / l;
    V.data[0] *= factor;
    V.data[1] *= factor;
    V.data[2] *= factor;
}

matrix transposeMatrix(matrix M) {
    matrix MT = makeMatrix(M.m, M.n);
    for (int i = 0; i < M.n; i++)
        for (int j = 0; j < M.m; j++)
            MT.data[j * MT.m + i] = M.data[i * M.m + j];
    return MT;
}