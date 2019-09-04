#include <stdio.h>
#include <math.h>
#include <assert.h>

/**
 * \brief An implementation of complex number
 * @param real Real part of complex number
 * @param imag Imaginary part of complex number
 */
struct complex {
    double real;
    double imag;
};

double discriminant(double a, double b, double c);

void input(double *a, double *b, double *c);

int solve(complex *solution1, complex *solution2, double a, double b, double c);

void printComplexNumber(complex z);


int main() {
    double a, b, c;
    printf("Enter coefficients a, b and c of quadratic equation ax^2+bx+c=0:\n");
    input(&a, &b, &c);

    complex solution1, solution2;
    int code;
    code = solve(&solution1, &solution2, a, b, c);
    if (code > 1) printf("Discriminant: %lg\n", discriminant(a, b, c));
    if (code == -1) {
        printf("Equation is linear, there is no roots");
    } else if (code == 1) {
        printf("Equation is linear, the root is x = %lg", solution1.real);
    } else if (code == 2) {
        printf("Quadratic equation has a zero discriminant and, therefore, single root x = %lg", solution1.real);
    } else if (code == 3) {
        printf("Roots of the quadratic equation are x1 = %lg and x2 = %lg", solution1.real, solution2.real);
    } else if (code == 4) {
        printf("Discriminant is negative, therefore quadratic equation has complex roots x1 = ");
        printComplexNumber(solution1);
        printf("and x2 = ");
        printComplexNumber(solution2);
        printf("\n");

    }
}

/**
 * \brief Implementation of reading function
 * Function designed for handling user's input
 * @param a Pointer to variable representing first coefficient of quadratic equation
 * @param b Pointer to variable representing second coefficient of quadratic equation
 * @param c Pointer to variable representing third coefficient of quadratic equation
 */
void input(double *a, double *b, double *c) {
    assert(a);
    assert(b);
    assert(c);
    char ch;
    while (scanf("%lg %lg %lg", a, b, c) != 3) {
        fflush(stdin);
    }
}

/**
 * Function that calculates discriminant of quadratic equation
 * @param a First coefficient of quadratic equation
 * @param b Second coefficient of quadratic equation
 * @param c Third coefficient of quadratic equation
 * @return Discriminant of quadratic equation
 */
double discriminant(double a, double b, double c) {
    return b * b - 4 * a * c;
}

/**
 * Function designed for solving equation basing on its type. Supports linear and quadratic equation with real and complex roots.
 * @param solution1 Pointer to the variable for the first solution
 * @param solution2 Pointer to the variable for the first solution
 * @param a First coefficient of quadratic equation
 * @param b Second coefficient of quadratic equation
 * @param c Third coefficient of quadratic equation
 * @return Code describing type of equation and information about any errors
 * |Code|Description|
 * |-------|--------|
 * |-1|Linear equation with no solution i. e. with zero second coeffitient|
 * |1|Linear equation with a solution|
 * |2|Quadratic equation with a single root|
 * |3|Quadratic equation with two real roots|
 * |4|Quadratic equation with two complex roots|
 */
int solve(complex *solution1, complex *solution2, double a, double b, double c) {
    assert(solution1);
    assert(solution2);
    if (a == 0) {
        if (b == 0) return -1;
        solution1->imag = 0;
        solution1->real = -c / b;
        solution2->imag = 0;
        solution2->real = -c / b;
        return 1;
    } else {
        double disc = discriminant(a, b, c);
        if (disc >= 0) {
            solution1->imag = 0;
            solution1->real = (-b + sqrt(disc)) / (2 * a);
            solution2->imag = 0;
            solution2->real = (-b - sqrt(disc)) / (2 * a);
            if (disc == 0) return 2;
            else return 3;
        } else {
            solution1->imag = sqrt(-disc) / (2 * a);
            solution1->real = -b / (2 * a);
            solution2->imag = -sqrt(-disc) / (2 * a);
            solution2->real = -b / (2 * a);
            return 4;
        }
    }
}


/**
 * Function that correctly prints complex number
 * @param z Complex number
 */
void printComplexNumber(complex z) {
    if (z.real != 0) printf("%lg ", z.real);
    if (z.imag >= 0) printf("+ ");
    printf("%lgi\n", z.imag);
}