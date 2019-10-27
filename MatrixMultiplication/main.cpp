#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

struct matrix {
    int n;
    int m;
    int *data;
};

matrix makeMatrix(int n, int m);

matrix mulMatrix(matrix mat1, matrix mat2);

matrix makeRotMatrix(char axis, int angle);

matrix shtrassenMulMatrix(matrix mat1, matrix mat2);

matrix mergeMatrices(matrix m11, matrix m12, matrix m21, matrix m22);

matrix addMatrix(matrix mat1, matrix mat2);

matrix subMatrix(matrix mat1, matrix mat2);

int getMatrix(matrix M, int i, int j);

void splitMatrix(matrix mat, matrix *m11, matrix *m12, matrix *m21, matrix *m22);

void resizeVectorByValue(matrix V, int value);

void printMatrix(FILE *f, matrix M);

void setMatrix(matrix M, int i, int j, int val);

matrix transposeMatrix(matrix M);

const int THRESHOLD = 1;

int main() {
    FILE *input = fopen("input.txt", "r");
    FILE *output = fopen("output.txt", "w");
    int n = 0;
    int value = 0;
    fscanf(input, "%d", &n);
    matrix M1 = makeMatrix(n, n);
    matrix M2 = makeMatrix(n, n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            fscanf(input, "%d", &value);
            setMatrix(M1, i, j, value);
        }
        fscanf(input, "\n");
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            fscanf(input, "%d", &value);
            setMatrix(M2, i, j, value);
        }
        fscanf(input, "\n");
    }
    matrix answer = shtrassenMulMatrix(M1, M2);
    printMatrix(output, answer);
    fclose(input);
    fclose(output);
}

matrix makeMatrix(int n, int m) {
    matrix M = {};
    M.n = n;
    M.m = m;
    M.data = (int *) calloc(n * m, sizeof(int));
    return M;
}

matrix shtrassenMulMatrix(matrix mat1, matrix mat2) {
    assert(mat1.m == mat2.n);
    assert(mat1.data);
    assert(mat2.data);
    assert(mat1.m == mat1.n);

    if (mat1.n <= THRESHOLD)
        return mulMatrix(mat1, mat2);

    matrix a11 = {};
    matrix a12 = {};
    matrix a21 = {};
    matrix a22 = {};

    matrix b11 = {};
    matrix b12 = {};
    matrix b21 = {};
    matrix b22 = {};

    splitMatrix(mat1, &a11, &a12, &a21, &a22);
    splitMatrix(mat2, &b11, &b12, &b21, &b22);

    matrix p1 = shtrassenMulMatrix(addMatrix(a11, a22), addMatrix(b11, b22));
    matrix p2 = shtrassenMulMatrix(addMatrix(a21, a22), b11);
    matrix p3 = shtrassenMulMatrix(a11, subMatrix(b12, b22));
    matrix p4 = shtrassenMulMatrix(a22, subMatrix(b21, b11));
    matrix p5 = shtrassenMulMatrix(addMatrix(a11, a12), b22);
    matrix p6 = shtrassenMulMatrix(subMatrix(a21, a11), addMatrix(b11, b12));
    matrix p7 = shtrassenMulMatrix(subMatrix(a12, a22), addMatrix(b21, b22));

    matrix c11 = addMatrix(addMatrix(p1, p4), subMatrix(p7, p5));
    matrix c12 = addMatrix(p3, p5);
    matrix c21 = addMatrix(p2, p4);
    matrix c22 = addMatrix(subMatrix(p1, p2), addMatrix(p3, p6));
    return mergeMatrices(c11, c12, c21, c22);
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
    int res;
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
            fprintf(f, "%d ", getMatrix(M, i, j));
        }
        fprintf(f, "\n");
    }
}

void setMatrix(matrix M, int i, int j, int val) {
    assert(i < M.n);
    assert(j < M.m);
    assert(M.data);
    M.data[i * M.m + j] = val;
}

int getMatrix(matrix M, int i, int j) {
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

void resizeVectorByValue(matrix V, int value) {
    assert(V.n == 3);
    assert(V.m == 1);
    assert(V.data);
    int l = sqrt(V.data[0] * V.data[0] + V.data[1] * V.data[1] + V.data[2] * V.data[2]);
    int factor = (l + value) / l;
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